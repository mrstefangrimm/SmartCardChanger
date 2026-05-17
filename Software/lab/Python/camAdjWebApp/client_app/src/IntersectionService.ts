import type { Intersection } from "./Intersection";

export class IntersectionService {
    private readonly apiUrl = "/api/intersections";

    async getAll(): Promise<Intersection[]> {
        const response = await fetch(this.apiUrl);

        if (!response.ok) {
            throw new Error(`Failed to load intersections: ${response.status}`);
        }

        return await response.json() as Intersection[];
    }
}

