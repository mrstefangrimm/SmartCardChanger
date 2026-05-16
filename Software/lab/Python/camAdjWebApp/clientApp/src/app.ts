import { IntersectionService } from "./IntersectionService.js";
import { Intersection } from "./Intersection.js";

const service = new IntersectionService();

function updateLiveVideoSettings() {
  const checkbox = document.getElementById("video-enabled") as HTMLInputElement;

  if (!checkbox) {
    console.error('video-enabled element not found');
    return;
  }

  checkbox.addEventListener("change", async () => {
    const enabled = checkbox.checked;

    await fetch("/video_settings", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({ enabled })
    });
  });
}


async function updateIntersections(): Promise<void> {

  const listEl = document.getElementById('intersectionsList');

  if (!listEl) {
    console.error('intersectionsList element not found');
    return;
  }

  const intersections = await service.getAll();
  if (intersections.length === 0) {
    listEl.innerHTML = '<li>No intersections detected</li>';
    return;
  }

  listEl.innerHTML = renderIntersections(intersections);
}

function renderIntersections(intersections: Intersection[]): string {
  return intersections
    .map((pt, i) =>
      `<li class="intersection-item">
              Point ${i + 1}: x=${pt.x.toFixed(2)}, y=${pt.y.toFixed(2)}
          </li>`
    ).join('');
}

// Start updating on page load
document.addEventListener('DOMContentLoaded', () => {
  console.log('App initialized');

  updateIntersections();
  updateLiveVideoSettings();

  // Update every 500ms
  setInterval(updateIntersections, 500);
});
