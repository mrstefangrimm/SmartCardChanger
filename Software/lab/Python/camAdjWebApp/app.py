from flask import Flask, render_template, jsonify, request, send_from_directory
from task import *
from stream import Stream
from processing import *

import time
import os

app = Flask(
    __name__,
    static_folder="client_app/static",
    static_url_path="/static",
    template_folder="client_app/templates",
)

class Camera:
    def __init__(self):
        self.id: int = 0
        self.siumulated: bool = True
        self.active: bool = True

    def to_dict(self):
        return {"id": self.id, "siumulated": self.siumulated, "active": self.active}    

camera_store = [
    Camera()
]

# Processings
sizePositionRotateSkewFilter = SizePositionRotateSkewFilter(id=1, short_name="SPR", type="Filter", name="Size, Position, Rotate")
edgeFilter = EdgeFilter(id=2, short_name="EDG", type="Filter", name="Gaussian Blur and Canny Edge Detection")
convertToJpgProcessing = ConvertToJpgProcessing(id=3, short_name="JPG", type="Converter", name="Convert to JPEG")
houghLinesFeatureDetector = HoughLinesFeatureDetector(id=4, short_name="HGL", type="Analyzer", name="Hough transform line detection")

processing_store = [
    sizePositionRotateSkewFilter,
    edgeFilter,
    convertToJpgProcessing,
    houghLinesFeatureDetector,
]

# Streams
from_camara_stream = Stream(id=1, name="Captured Image from Camera", status="New")
adjusted_image_stream = Stream(id=2, name="Adjusted Image", status="New")
unused_scanline_processing_stream = Stream(id=3, name="Line Profile Image", status="New")
live_video_stream = Stream(id=4, name="Video Stream", status="New")

stream_store = [
    from_camara_stream,
    adjusted_image_stream,
    unused_scanline_processing_stream,
    live_video_stream,
]

# Tasks
#camera = CameraCapture(output_stream=from_camara_stream, camera_index=0, capture_interval=0.5)
camera = FakeCapture(output_stream=from_camara_stream)
liveImgOverlayTask = CamaraAdjustmentOverlay(input_stream=adjusted_image_stream, output_stream=live_video_stream, processing_store=processing_store)
carriageTask = CarriageDetection(input_stream=unused_scanline_processing_stream, output_stream=None, edgeFilter=edgeFilter)

@app.route("/")
def index():
    """Serve the main page."""
    return render_template("index.html")


@app.route("/camera_adjustments", methods=["GET", "POST"])
def camera_adjustments():
    if request.method == "GET":
        return jsonify(
            {
                "x": sizePositionRotateSkewFilter.x,
                "y": sizePositionRotateSkewFilter.y,
                "width": sizePositionRotateSkewFilter.width,
                "height": sizePositionRotateSkewFilter.height,
                "rtn": sizePositionRotateSkewFilter.rtn,
                "blur_enabled": edgeFilter.blur_enabled,
                "blur_kernel_size": edgeFilter.kernel_size,
                "canny_lower_threshold": edgeFilter.upper_threshold,
                "canny_upper_threshold": edgeFilter.lower_threshold,
            }
        )

    if request.method == "POST":
        data = request.json

        # roi_x_str = data.get('x')
        sizePositionRotateSkewFilter.x = data.get(
            "x"
        )  # int(roi_x_str) if roi_x_str is not None else None

        # roi_y_str = data.get('y')
        sizePositionRotateSkewFilter.y = data.get(
            "y"
        )  # int(roi_y_str) if roi_y_str is not None else None

        # roi_w_str = data.get('width')
        sizePositionRotateSkewFilter.width = data.get(
            "width"
        )  # int(roi_w_str) if roi_w_str is not None else None

        # roi_h_str = data.get('height')
        sizePositionRotateSkewFilter.height = data.get(
            "height"
        )  # int(roi_h_str) if roi_h_str is not None else None

        sizePositionRotateSkewFilter.rtn = data.get("rtn")

        edgeFilter.blur_enabled = data.get("blur_enabled")


        return jsonify(
            {
                "x": sizePositionRotateSkewFilter.x,
                "y": sizePositionRotateSkewFilter.y,
                "width": sizePositionRotateSkewFilter.width,
                "height": sizePositionRotateSkewFilter.height,
                "rtn": sizePositionRotateSkewFilter.rtn,
                "blur_enabled": edgeFilter.blur_enabled,
                "blur_kernel_size": edgeFilter.kernel_size,
                "canny_lower_threshold": edgeFilter.upper_threshold,
                "canny_upper_threshold": edgeFilter.lower_threshold,
            }
        )

@app.get("/api/cameras")
def get_cameras():
    return jsonify([c.to_dict() for c in camera_store])

@app.get("/api/processings")
def get_processings():
    return jsonify([p.to_dict() for p in processing_store])

@app.patch("/api/processings/<int:processing_id>")
def update_processing(processing_id):
    processing = next((p for p in processing_store if p.id == processing_id), None)
   
    if processing is None:
        return jsonify({"error": "Processing not found"}), 404

    data = request.get_json()

    if isinstance(processing, SizePositionRotateSkewFilter):
        sprs: SizePositionRotateSkewFilter = processing
        sprs.x = data["x"]
        sprs.y = data["y"]
        sprs.width = data["width"]
        sprs.height = data["height"]
        sprs.rtn = data["rtn"]

    return processing.to_dict(), 200


@app.route("/api/intersections")
def get_intersections():
    """API endpoint to get current intersections as JSON."""
    return jsonify(camera.get_intersections())


@app.route("/video_feed")
def video_feed():
    """Stream video with intersection points drawn."""

    def generate():
        while True:
            frame = from_camara_stream.get_first_frame() # camera.get_frame()
            
            frame = sizePositionRotateSkewFilter.run(frame) if sizePositionRotateSkewFilter else frame
  
            adjusted_image_stream.append(0, frame=frame)
            unused_scanline_processing_stream.append(0, frame=frame)

            video_feed_frame = live_video_stream.get_first_frame()

            if video_feed_frame:
                yield (
                    b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + video_feed_frame + b"\r\n"
                )
                time.sleep(0.5)
            else:
                time.sleep(1)

    return app.response_class(
        generate(), mimetype="multipart/x-mixed-replace; boundary=frame"
    )


@app.route("/video_settings", methods=["GET", "POST"])
def video_settings():
    if request.method == "GET":
        # Return current values
        return jsonify(
            {
                "enabled": liveImgOverlayTask.video_show_line_profile,
            }
        )

    if request.method == "POST":
        data = request.json
        liveImgOverlayTask.video_show_line_profile = data.get("enabled")

        return jsonify({"status": "ok", "enabled": liveImgOverlayTask.video_show_line_profile})


if __name__ == "__main__":
    print("Starting camera...")
    camera.start()
    time.sleep(2)
    print("Camera started, starting Flask app...")

    liveImgOverlayTask.start()
    carriageTask.start()
    try:
        #app.run(debug=False, host="0.0.0.0", port=5000, threaded=True)
        app.run(debug=True, port=5000, threaded=True)
    finally:
        camera.stop()
