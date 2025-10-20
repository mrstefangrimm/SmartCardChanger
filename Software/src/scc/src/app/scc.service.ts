import { Injectable } from "@angular/core"

@Injectable({ providedIn: 'root' })
export class SccService {

  showAsTransparent = false
  showReaderAndCards = false
  visiblitiesOpen: boolean = false

  constructor() {
    console.info(SccService.name, "c'tor")
  }
}
