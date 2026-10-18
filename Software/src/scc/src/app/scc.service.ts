import { Injectable } from "@angular/core"

@Injectable({ providedIn: 'root' })
export class SccService {

  showAsTransparent = false
  showReaderAndCards = false
  visibilitiesOpen: boolean = false

  constructor() {
    console.info(SccService.name, "c'tor")
  }
}
