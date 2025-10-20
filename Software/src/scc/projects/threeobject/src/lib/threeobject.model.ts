import { Observable } from 'rxjs'
import * as THREE from 'three'
import { Material, Object3D, Quaternion, Vector3 } from 'three'

export class ThreeObject {

  fromWorldToLocalOrigin: Vector3 // From world space origin to the local orign
  position: Vector3 // From the local origin to the center of the object
  material: Material
  geometry: THREE.BufferGeometry

  private posX = 0
  private posZ = 0
  private rtnY = 0
  private rtnZ = 0
  private object: Object3D

  build(): Observable<Object3D> {
    return new Observable<Object3D>(subscriber => {

      this.object = new THREE.Mesh(this.geometry, this.material)

      this.object.translateX(this.fromWorldToLocalOrigin.x + this.position.x)
      this.object.translateY(this.fromWorldToLocalOrigin.y + this.position.y)
      this.object.translateZ(this.fromWorldToLocalOrigin.z + this.position.z)

      subscriber.next(this.object)
    })
  }

  dispose() {
    this.material.dispose()
    this.geometry.dispose()
  }

  setPosX(pos: number) {
    const distance = (pos - this.posX)
    this.object.translateX(distance)
    this.posX = pos
  }

  setPosZ(pos: number) {
    const distance = (pos - this.posZ)
    this.object.translateZ(distance)
    this.posZ = pos
  }

  setRtnY(rtn: number) {
    this.object.translateX(-this.position.x - this.posX)
    this.object.translateY(-this.position.y)
    this.object.translateZ(-this.position.z - this.posZ)

    const angle = (rtn - this.rtnY)
    this.object.rotateY(angle)
    this.rtnY = rtn

    this.object.translateX(this.position.x + this.posX)
    this.object.translateY(this.position.y)
    this.object.translateZ(this.position.z + this.posZ)
  }

  setRtnZ(rtn: number) {
    this.object.translateX(-this.position.x - this.posX)
    this.object.translateY(-this.position.y)
    this.object.translateZ(-this.position.z - this.posZ)

    // the object's z axis back to the local cooridate system's z axis, then rotate around z.
    this.object.rotateY(-this.rtnY)

    const angle = (rtn - this.rtnZ)
    this.object.rotateZ(angle)
    this.rtnZ = rtn

    this.object.rotateY(this.rtnY)

    this.object.translateX(this.position.x + this.posX)
    this.object.translateY(this.position.y)
    this.object.translateZ(this.position.z + this.posZ)
  }

  setVisible(show: boolean) {
    this.object.visible = show
  }

  setMaterial(mat: Material) {
    this.material = mat
    this.object.traverse(child => {
      if (child instanceof THREE.Mesh) {
        child.material = mat
      }
    })
  }
}
