import { signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { Component, ViewChild, ElementRef, OnInit, OnDestroy, AfterViewInit } from '@angular/core'
import { SccEngine3dService } from './sccengine3d.service'
import { SccService } from './scc.service'
import { Vector3 } from 'three'
import { CommonModule } from '@angular/common'


@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet, CommonModule],
  templateUrl: './app.html',
  styleUrl: './app.css'
})
export class App implements OnInit, OnDestroy, AfterViewInit {
  protected readonly title = signal('scc');

  @ViewChild('rendererCanvas', { static: true })
  rendererCanvas: ElementRef<HTMLCanvasElement>

  constructor(
    public context: SccService,
    private readonly engine3d: SccEngine3dService) {
    console.info(App.name, "c'tor")  
  }

  ngOnInit(): void {
    console.info(App.name, "ngOnInit")

    console.debug(App.name, window.innerWidth, window.innerHeight)

    const size = SccEngine3dService.calcContentSize()  
    this.engine3d.createScene(this.rendererCanvas, size.width, size.height)
    this.engine3d.animate()
  }

  ngAfterViewInit(): void {
    console.info(App.name, "ngAfterViewInit")
  }

  ngOnDestroy(): void {
    console.info(App.name, "ngOnDestroy")
    this.engine3d.ngOnDestroy()
  }

  onLngChanged(event) {
    console.debug('onLngChanged', event.value)

    const alpha = event.value / 180 * Math.PI
    const rtnServoArm = alpha - Math.PI

    const a = 40
    const c = 175
    const c2 = 175 * 175
    
    const y = Math.sin(alpha) * a
    const x = Math.sqrt(c2 - y * y)
    const b = x - Math.cos(alpha) * a
 
    const magicAngFactor = 1.1
    const beta = Math.acos(x / c) * magicAngFactor

    const lng = b - a - c

    console.debug('alpha', alpha, 'rtnServoArm', rtnServoArm, 'x', x, 'y', y, 'c', c, 'beta', beta, 'lng', lng, 'b', b)

    this.engine3d.groupCarriageWood.translate(new Vector3(lng, 0, 0))
    this.engine3d.groupCarriageSilver.translate(new Vector3(lng, 0, 0))
    this.engine3d.groupCarriageAnthracite.translate(new Vector3(lng, 0, 0))
    this.engine3d.groupCarriageGray.translate(new Vector3(lng, 0, 0))
    this.engine3d.groupCarriageGreen.translate(new Vector3(lng, 0, 0))
    this.engine3d.groupCarriageTransparent.translate(new Vector3(lng, 0, 0))

    this.engine3d.groupServoArmExtensionWood.translate(new Vector3(lng, 0, 0))

    this.engine3d.groupServoArmWood.rotate(-rtnServoArm, new Vector3(0, 0, 1))
    this.engine3d.groupServoArmSilver.rotate(-rtnServoArm, new Vector3(0, 0, 1))
    this.engine3d.groupServoArmWhite.rotate(-rtnServoArm, new Vector3(0, 0, 1))

    this.engine3d.groupServoArmExtensionWood.rotate(-beta, new Vector3(0, 0, 1))
  }

  onRtnChanged(event) {
    console.debug(event.value)

    const rtn = (event.value - 90) / 180 * Math.PI
    const axis = new Vector3(0, 0, 1)
    
    this.engine3d.groupRotatorWood.rotate(rtn, axis)
    this.engine3d.groupRotatorSilver.rotate(rtn, axis)
    this.engine3d.groupRotatorWhite.rotate(rtn, axis)
    this.engine3d.groupRotatorCardboard.rotate(rtn, axis)
    this.engine3d.groupRotatorSmartcards.rotate(rtn, axis)
  }

  onTransparentChecked() {
    const checked = !this.context.showAsTransparent
    console.info(App.name, "onTransparentChecked", checked)
    this.engine3d.setTransparent(checked)
    this.context.showAsTransparent = checked
  }

  onReaderAndCardsChecked() {
    const checked = !this.context.showReaderAndCards
    console.info(App.name, "onReaderAndCardsChecked", checked)
    this.engine3d.groupRotatorSmartcards.setVisible(checked)
    this.engine3d.groupCarriageGray.setVisible(checked)
    this.engine3d.groupCarriageGreen.setVisible(checked)
    this.engine3d.groupCarriageTransparent.setVisible(checked)
    this.context.showReaderAndCards = checked
  }
}
