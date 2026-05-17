export class IntersectionService {
    constructor() {
        this.apiUrl = "/api/intersections";
    }
    async getAll() {
        const response = await fetch(this.apiUrl);
        if (!response.ok) {
            throw new Error(`Failed to load intersections: ${response.status}`);
        }
        return await response.json();
    }
}
