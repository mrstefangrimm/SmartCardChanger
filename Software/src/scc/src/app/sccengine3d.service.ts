import * as THREE from 'three'
import { Vector3 } from 'three'
import { ElementRef, Injectable, NgZone, OnDestroy } from '@angular/core'
import { OrbitControls } from 'three/addons/controls/OrbitControls.js'
import { LoadableObject, LoadedObject, NotLoadedObject } from 'threeobject'
import { Observable } from 'rxjs'
//import { useTexture } from '@react-three/fiber'; // Or use TextureLoader
import { MeshStandardMaterial } from 'three';
   
@Injectable({providedIn: 'root'})
export class SccEngine3dService implements OnDestroy {

  private readonly zero: Vector3 = new Vector3(0, 0, 0)
  
  private renderer: THREE.WebGLRenderer
  private camera: THREE.PerspectiveCamera
  private scene: THREE.Scene
  private frameId: number = null

  private meshes = []

  private materialFelt: THREE.Material
  private materialWhite: THREE.Material
  private materialAnthracite: THREE.Material
  private materialSilver: THREE.Material
  private materialGold: THREE.Material
  private materialCardboard: THREE.Material
  private materialWood: THREE.Material
  private materialXray: THREE.Material
  private materialGreen: THREE.Material

  private groupStaticWood: LoadableObject
  private groupStaticSilver: LoadableObject
  private groupStaticFelt: LoadableObject
  private groupStaticAnthracite: LoadableObject
  
  groupCarriageWood: LoadableObject 
  groupCarriageSilver: LoadableObject
  groupCarriageAnthracite: LoadableObject
  groupCarriageGray: LoadableObject
  groupCarriageGreen: LoadableObject
  groupCarriageTransparent: LoadableObject

  groupServoArmWood: LoadableObject
  groupServoArmSilver: LoadableObject
  groupServoArmWhite: LoadableObject

  groupServoArmExtensionWood: LoadableObject

  groupRotatorWood: LoadableObject
  groupRotatorSilver: LoadableObject
  groupRotatorWhite: LoadableObject
  groupRotatorCardboard: LoadableObject
  groupRotatorSmartcards: LoadableObject
  
  public constructor(private ngZone: NgZone) {
    const baseUrl = document.getElementsByTagName('base')[0].href
    console.info(SccEngine3dService.name, "c'tor", "base url", baseUrl)
  }

  ngOnDestroy() {
    console.info(SccEngine3dService.name, "ngOnDestroy")
    if (this.frameId != null) {
      cancelAnimationFrame(this.frameId)
    }
    this.frameId = null
    this.meshes.forEach(m => this.scene.remove(m))
    this.materialFelt.dispose()
    this.materialWhite.dispose()
    this.materialAnthracite.dispose()
    this.materialSilver.dispose()
    this.materialGold.dispose()
    this.materialCardboard.dispose()
    this.materialWood.dispose()
    this.materialXray.dispose()
    this.materialGreen.dispose()
    this.groupStaticWood.dispose()
    this.groupStaticSilver.dispose()
    this.groupStaticFelt.dispose()
    this.groupStaticAnthracite.dispose()
    this.groupCarriageWood.dispose()
    this.groupCarriageSilver.dispose()
    this.groupCarriageAnthracite.dispose()
    this.groupCarriageGray.dispose()
    this.groupCarriageGreen.dispose()
    this.groupCarriageTransparent.dispose()
    this.groupServoArmWood.dispose()
    this.groupServoArmSilver.dispose()
    this.groupServoArmWhite.dispose()
    this.groupServoArmExtensionWood.dispose()
    this.groupRotatorWood.dispose()
    this.groupRotatorSilver.dispose()
    this.groupRotatorWhite.dispose()
    this.groupRotatorCardboard.dispose()
    this.groupRotatorSmartcards.dispose()
  }

  createScene(canvas: ElementRef<HTMLCanvasElement>, width: number, height: number) {
    console.info(SccEngine3dService.name, "createScene")

    const w = width
    const h = height
    console.debug(w, h)

    this.renderer = new THREE.WebGLRenderer({
      canvas: canvas.nativeElement,
      alpha: true,     // transparent background
      antialias: true, // smooth edges
    });
    this.renderer.setSize(w, h)
 
    // create the scene
    this.scene = new THREE.Scene() 

    // var axesHelper = new THREE.AxesHelper(50);
    // this.scene.add( axesHelper );
 
    this.camera = new THREE.PerspectiveCamera(40, w / h, 0.1, 1500)
    this.camera.position.set(-400, 250, 300)
    this.scene.add(this.camera)

    const controls = new OrbitControls(this.camera, this.renderer.domElement)
    controls.minDistance = 50
    controls.maxDistance = 1000
    controls.target = new Vector3(22.5, 57.5 , -70)
    controls.update()
    
    // const light = new THREE.PointLight(0xffffff, 10, 0, 0)
    // light.position.set(300, -700, 0)
    // this.scene.add(light)
    
    const light2 = new THREE.PointLight(0xffffff, 10, 0, 0)
    light2.position.set(0, 500, 300)
    this.scene.add(light2)

    var hemiLight = new THREE.HemisphereLight()
    this.scene.add(hemiLight);

    this.materialFelt = new THREE.MeshStandardMaterial({
      color: 0xEDEDED,
      roughness: 0.5,
      metalness: 0,
    })   
    this.materialWhite = new THREE.MeshStandardMaterial({
      color: 0xFFFFFF,
      metalness: 0,
    })
    this.materialAnthracite = new THREE.MeshStandardMaterial({
      color: 0x383E42,
      roughness: 0.5,
      metalness: 0,
    })
    this.materialSilver = new THREE.MeshStandardMaterial({
      color: 0xAAA9AD,
      roughness: 0.5,
      metalness: 1.0,
    })
    this.materialGold = new THREE.MeshStandardMaterial({
      color: 0xFFD700,
      roughness: 0.5,
      metalness: 1.0,
    })
    this.materialCardboard = new THREE.MeshStandardMaterial({
      color: 0xDCDCDC,
      roughness: 0.5,
      metalness: 1.0,
    })
    this.materialGreen = new THREE.MeshStandardMaterial({
      color: 0x00FF00,
      roughness: 0.5,
      metalness: 1.0,
    })
    // const map = new THREE.TextureLoader().load('assets/wood.png')
    // const wood = new THREE.SpriteMaterial({ map: map, color: 0xffffff })
    
    // const sprite = new THREE.Sprite(wood)
    // sprite.scale.set(300, 200, 1)
    // this.scene.add(sprite)

    // this.materialWood = wood
    // const woodTexture = useTexture('/path/to/wood.jpg');
    // const woodMaterial = new MeshStandardMaterial({
    //   map: woodTexture,
    //   metalness: 0.0,
    // });

    this.materialWood = new THREE.MeshStandardMaterial({
      color: 0xBA8C63,
      roughness: 1.4,
      metalness: 0,
    })
    this.materialXray = new THREE.MeshPhysicalMaterial({
      color: 0xAAAAAA,
      metalness: 0,
      roughness: 0,
      alphaTest: 0.5,
      depthWrite: false,
      transmission: 0.6,
      opacity: 0.5,
      transparent: true,
    })

    this.loadAndAdd(this.zero, this.materialWood, 'assets/SCC-VirmsGroupStaticWood.obj').subscribe({
      next: obj => this.groupStaticWood = obj
    })
    this.loadAndAdd(this.zero, this.materialSilver, 'assets/SCC-VirmsGroupStaticSilver.obj').subscribe({
      next: obj => this.groupStaticSilver = obj
    })
    this.loadAndAdd(this.zero, this.materialFelt, 'assets/SCC-VirmsGroupStaticFelt.obj').subscribe({
      next: obj => this.groupStaticFelt = obj
    })
    this.loadAndAdd(this.zero, this.materialAnthracite, 'assets/SCC-VirmsGroupStaticAnthracite.obj').subscribe({
      next: obj => this.groupStaticAnthracite = obj
    })
    this.loadAndAdd(this.zero, this.materialWood, 'assets/SCC-VirmsGroupCarriageWood.obj').subscribe({
      next: obj => this.groupCarriageWood = obj
    })
    this.loadAndAdd(this.zero, this.materialSilver, 'assets/SCC-VirmsGroupCarriageSilver.obj').subscribe({
      next: obj => this.groupCarriageSilver = obj
    })
    this.loadAndAdd(this.zero, this.materialAnthracite, 'assets/SCC-VirmsGroupCarriageAnthracite.obj').subscribe({
      next: obj => this.groupCarriageAnthracite = obj
    })
    this.loadInvisibleAndAdd(this.zero, this.materialCardboard, 'assets/SCC-VirmsGroupCarriageGray.obj').subscribe({
      next: obj => this.groupCarriageGray = obj
    })
    this.loadInvisibleAndAdd(this.zero, this.materialGreen, 'assets/SCC-VirmsGroupCarriageGreen.obj').subscribe({
      next: obj => this.groupCarriageGreen = obj
    })
    this.loadInvisibleAndAdd(this.zero, this.materialXray, 'assets/SCC-VirmsGroupCarriageTransparent.obj').subscribe({
      next: obj => this.groupCarriageTransparent = obj
    })      
    this.loadAndAdd(new Vector3(-69.8, -70, 0), this.materialWood, 'assets/SCC-VirmsGroupServoArmWood.obj').subscribe({
      next: obj => this.groupServoArmWood = obj
    })
    this.loadAndAdd(new Vector3(-69.8, -70, 0), this.materialSilver, 'assets/SCC-VirmsGroupServoArmSilver.obj').subscribe({
      next: obj => this.groupServoArmSilver = obj
    })
    this.loadAndAdd(new Vector3(-69.8, -70, 0), this.materialWhite, 'assets/SCC-VirmsGroupServoArmWhite.obj').subscribe({
      next: obj => this.groupServoArmWhite = obj
    })
    this.loadAndAddWithModelOffset(new Vector3(0, 0, 0), new Vector3(265, 17.7, -70), this.materialWood, 'assets/SCC-VirmsGroupServoArmExtensionWood.obj').subscribe({
      next: obj => this.groupServoArmExtensionWood = obj
    })
    this.loadAndAdd(new Vector3(-22.5, -70, 0), this.materialWood, 'assets/SCC-VirmsGroupRotatorWood.obj').subscribe({
      next: obj => this.groupRotatorWood = obj
    })
    this.loadAndAdd(new Vector3(-22.5, -70, 0), this.materialSilver, 'assets/SCC-VirmsGroupRotatorSilver.obj').subscribe({
      next: obj => this.groupRotatorSilver = obj
    })
    this.loadAndAdd(new Vector3(-22.5, -70, 0), this.materialWhite, 'assets/SCC-VirmsGroupRotatorWhite.obj').subscribe({
      next: obj => this.groupRotatorWhite = obj
    })
    this.loadAndAdd(new Vector3(-22.5, -70, 0), this.materialCardboard, 'assets/SCC-VirmsGroupRotatorCardboard.obj').subscribe({
      next: obj => this.groupRotatorCardboard = obj
    })
    this.loadInvisibleAndAdd(new Vector3(-22.5, -70, 0), this.materialGold, 'assets/SCC-VirmsGroupRotatorSmartcards.obj').subscribe({
      next: obj => this.groupRotatorSmartcards = obj
    })
  }

  private loadInvisibleAndAdd(origin: Vector3, material: THREE.Material, asset: string) : Observable<LoadableObject> {
    return new Observable<LoadableObject>(subscriber => {
      var model = LoadedObject.createFromSketchup(origin, this.zero, material)
      model.loadInvisible(asset).subscribe({
        next: object3d => {
          this.scene.add(object3d)
          subscriber.next(model)
        },
        error: () => {
          console.warn(SccEngine3dService.name, "loadInvisibleAndAdd", asset, "not shown.")
          subscriber.next(new NotLoadedObject())
        }
      })
    })
  }

  private loadAndAdd(origin: Vector3, material: THREE.Material, asset: string) : Observable<LoadableObject> {
    return new Observable<LoadableObject>(subscriber => {
      var model = LoadedObject.createFromSketchup(origin, this.zero, material)
      model.load(asset).subscribe({
        next: object3d => {
          this.scene.add(object3d)
          subscriber.next(model)
        },
        error: () => {
          console.warn(SccEngine3dService.name, "loadAndAdd", asset, "not shown.")
          subscriber.next(new NotLoadedObject())
        }
      })
    })
  }

  private loadAndAddWithModelOffset(origin: Vector3, position: Vector3, material: THREE.Material, asset: string) : Observable<LoadableObject> {
    return new Observable<LoadableObject>(subscriber => {
      var model = LoadedObject.createFromSketchup(origin, position, material)
      model.load(asset).subscribe({
        next: object3d => {
          this.scene.add(object3d)
          subscriber.next(model)
        },
        error: () => {
          console.warn(SccEngine3dService.name, "loadAndAddWithModelOffset", asset, "not shown.")
          subscriber.next(new NotLoadedObject())
        }
      })
    })
  }

  animate() {
    console.debug(SccEngine3dService.name, "animate")

    // Run outside angular zones, because it could trigger heavy changeDetection cycles.
    this.ngZone.runOutsideAngular(() => {
      if (document.readyState !== 'loading') {
        this.render()
      }
      else {
        window.addEventListener('DOMContentLoaded', () => {
          this.render()
        })
      }
      window.addEventListener('resize', () => {
        this.resize()
      })
    })
  }

  private render() {
    this.frameId = requestAnimationFrame(() => {
      this.render()
    })
    this.renderer.render(this.scene, this.camera)
  }

  private resize() {
    const size = SccEngine3dService.calcContentSize()
   
    this.camera.aspect = size.width / size.height
    this.camera.updateProjectionMatrix()

    this.renderer.setSize(size.width, size.height)
  }

  static calcContentSize() : {width: number; height: number} {

    const sideNavWidth = window.innerWidth > 620 
                         ? window.innerWidth * 0.2 + 14
                         : 18
    const contentWidth = window.innerWidth - sideNavWidth

    const footerHeight = window.innerWidth > 620 
                         ? window.innerHeight * 0.1
                         :  window.innerHeight * 0.3
    const contentHeight = window.innerHeight - footerHeight

    return { width: contentWidth, height: contentHeight}
  }

  setTransparent(transparent: boolean) {
    console.info(SccEngine3dService.name, "setTransparent", transparent)
    if (transparent) {
      this.groupStaticWood.setMaterial(this.materialXray)
      this.groupStaticSilver.setMaterial(this.materialXray)
      this.groupStaticFelt.setMaterial(this.materialXray)
      this.groupStaticAnthracite.setMaterial(this.materialXray)
      this.groupCarriageWood.setMaterial(this.materialXray)
      this.groupCarriageSilver.setMaterial(this.materialXray)
      this.groupCarriageAnthracite.setMaterial(this.materialXray)
      this.groupCarriageGray.setMaterial(this.materialXray)
      this.groupCarriageGreen.setMaterial(this.materialXray)
      this.groupCarriageTransparent.setMaterial(this.materialXray)
      this.groupServoArmWood.setMaterial(this.materialXray)
      this.groupServoArmSilver.setMaterial(this.materialXray)
      this.groupServoArmWhite.setMaterial(this.materialXray)
      this.groupServoArmExtensionWood.setMaterial(this.materialXray)
      this.groupRotatorWood.setMaterial(this.materialXray)
      this.groupRotatorSilver.setMaterial(this.materialXray)
      this.groupRotatorWhite.setMaterial(this.materialXray)
      this.groupRotatorCardboard.setMaterial(this.materialXray)    
      this.groupRotatorSmartcards.setMaterial(this.materialXray)  
    }
    else {
      this.groupStaticWood.setMaterial(this.materialWood)
      this.groupStaticSilver.setMaterial(this.materialSilver)
      this.groupStaticFelt.setMaterial(this.materialFelt)
      this.groupStaticAnthracite.setMaterial(this.materialAnthracite)
      this.groupCarriageWood.setMaterial(this.materialWood)
      this.groupCarriageSilver.setMaterial(this.materialSilver)
      this.groupCarriageAnthracite.setMaterial(this.materialAnthracite)
      this.groupCarriageGray.setMaterial(this.materialCardboard)
      this.groupCarriageGreen.setMaterial(this.materialGreen)
      this.groupCarriageTransparent.setMaterial(this.materialXray)
      this.groupServoArmWood.setMaterial(this.materialWood)
      this.groupServoArmSilver.setMaterial(this.materialSilver)
      this.groupServoArmWhite.setMaterial(this.materialWhite)
      this.groupServoArmExtensionWood.setMaterial(this.materialWood)
      this.groupRotatorWood.setMaterial(this.materialWood)
      this.groupRotatorSilver.setMaterial(this.materialSilver)
      this.groupRotatorWhite.setMaterial(this.materialWhite)
      this.groupRotatorCardboard.setMaterial(this.materialCardboard)
      this.groupRotatorSmartcards.setMaterial(this.materialGold)
    }
  }
}
