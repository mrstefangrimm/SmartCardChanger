export class CameraAdjustmentsService {
    constructor() {
        this.apiUrl = "/camera_adjustments";
    }
    async get() {
        const response = await fetch(this.apiUrl);
        if (!response.ok) {
            throw new Error(`Failed to load intersections: ${response.status}`);
        }
        return await response.json();
    }
}
