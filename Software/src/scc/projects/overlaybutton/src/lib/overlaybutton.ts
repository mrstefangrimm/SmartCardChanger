import { Component, EventEmitter, Output } from '@angular/core';

@Component({
  selector: 'lib-overlaybutton',
  standalone: true,
  imports: [],
  templateUrl: './overlaybutton.html',
  styleUrl: './overlaybutton.css',
})
export class Overlaybutton {

  @Output() notify: EventEmitter<OverlaybuttonClickArgs> = new EventEmitter<OverlaybuttonClickArgs>();

  open: boolean = false

  onButtonClick() : void {
    this.open = !this.open

    this.notify.emit(new OverlaybuttonClickArgs(this.open))
  }
}

export class OverlaybuttonClickArgs {
  constructor(public open: boolean) {
  }
}
