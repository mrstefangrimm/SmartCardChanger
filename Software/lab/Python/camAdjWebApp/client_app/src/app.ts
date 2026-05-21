import { IntersectionService } from "./IntersectionService.js";
import { Intersection } from "./Intersection.js";

const intersectionService = new IntersectionService();

function updateLiveVideoSettings() {
  let checkbox = document.getElementById("video-enabled") as HTMLInputElement;

  fetch('/video_settings')
    .then(response => response.json())
    .then(data => {
      // let video_enabled = document.getElementById("video-enabled") as HTMLInputElement;

      console.log('API Response:', data);  // Log the entire response
      // checkbox.value = data.enabled;
    })
    .catch(error => console.error('Error fetching video_settings:', error));

  checkbox.addEventListener("change", async () => {
    const enabled = checkbox.checked;

    await fetch("/video_settings", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({ enabled: enabled })
    });
  });
}


function fetchLiveVideoAdjustments() {
  const applyButton = document.getElementById("apply_roi") as HTMLInputElement;

  fetch('/camera_adjustments')
    .then(response => response.json())
    .then(data => {
      let roi_x = document.getElementById("roi_x") as HTMLInputElement;
      let roi_y = document.getElementById("roi_y") as HTMLInputElement;
      let roi_width = document.getElementById("roi_width") as HTMLInputElement;
      let roi_height = document.getElementById("roi_height") as HTMLInputElement;

      // console.log('API Response:', data);  // Log the entire response
      // console.log('roi_x value:', data.x);  // Log just roi_x
      roi_x.value = data.x;
      roi_y.value = data.y;
      roi_width.value = data.width;
      roi_height.value = data.height;
    })
    .catch(error => console.error('Error fetching camera_adjustments:', error));

  // Handle the Apply button click
  applyButton.addEventListener('click', function () {
    const roi_x = document.getElementById("roi_x") as HTMLInputElement;
    const roi_y = document.getElementById("roi_y") as HTMLInputElement;
    const roi_width = document.getElementById("roi_width") as HTMLInputElement;
    const roi_height = document.getElementById("roi_height") as HTMLInputElement;
    const rtn = document.getElementById("rtn") as HTMLInputElement;

    if (!roi_x) {
      alert('Please enter a value');
      return;
    }

    fetch('/api/processings/1', {
      method: 'PATCH',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        x: Number.parseFloat(roi_x.value),
        y: Number.parseFloat(roi_y.value),
        width: Number.parseFloat(roi_width.value),
        height: Number.parseFloat(roi_height.value),
        rtn: Number.parseFloat(rtn.value),
      })
    })
      .then(response => response.json())
      .then(data => {
        // alert('ROI updated successfully!');
        console.log('Response:', data);
      })
      .catch(error => console.error('Error updating ROI:', error));
  });
}

async function updateIntersections(): Promise<void> {

  const listEl = document.getElementById('intersectionsList');

  if (!listEl) {
    console.error('intersectionsList element not found');
    return;
  }

  const intersections = await intersectionService.getAll();
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
  //updateLiveVideoAdjustments();
  fetchLiveVideoAdjustments();

  // Update every 500ms
  //setInterval(updateIntersections, 500);
  //setInterval(updateLiveVideoAdjustments, 500);
});
