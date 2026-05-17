import type { CameraAdjustments } from "./CameraAdjustments";

export class CameraAdjustmentsService {
  private readonly apiUrl = "/camera_adjustments";

  async get(): Promise<CameraAdjustments[]> {
    const response = await fetch(this.apiUrl);

    if (!response.ok) {
      throw new Error(`Failed to load intersections: ${response.status}`);
    }

    return await response.json() as CameraAdjustments[];
  }


}
