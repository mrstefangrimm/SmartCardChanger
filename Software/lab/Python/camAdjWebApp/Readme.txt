npm install --save-dev typescript ts-loader webpack webpack-cli


npm run build
or automatic rebuilding while developing:
npm run watch

python -m pip install opencv-python
python -m pip install flask
python -m flask --version

pip install pytest
py -m pytest tests/utils/test_frame_buffer.py
py -m pytest tests/utils/test_frame_buffer.py::TestFrameBuffer::test_push_single_frame -v


my_project/
├── app.py
├── camera.py
├── hough_lines.py
├── package.json
├── tsconfig.json
├── templates/
│   └── index.html
├── src/
│   └── app.ts
└── static/
    └── js/
        └── app.js (generated)




----
curl -X PATCH http://localhost:5000/api/processings/1 -H "Content-Type: application/json" -d '{ "x": 300, "y": 50, "width": 1800, "height": 1000, "rtn": 5 }'

curl -X PATCH http://localhost:5000/api/processings/2 -H "Content-Type: application/json" -d '{ "enabled": true, "blurEnabled": true, "lowerThreshold": 60, "upperThreshold": 100 }'