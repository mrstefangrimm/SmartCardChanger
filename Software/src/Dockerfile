FROM node:22-alpine

# Build docker image
# ------------------
# cd scc
# docker build -t scc_dev .

# Run container
# -------------
# docker run -p 4201:4200 -v $HOME\repos\SmartCardChanger\Software\Src\scc:/mnt/scc -it --rm --entrypoint sh scc_dev
# docker run -p 4201:4200 -v $HOME\Documents\github\SmartCardChanger\Software\src\scc:/mnt/scc -it --rm --entrypoint sh scc_dev

# Create Angular project
# ----------------------
# ng new scc
# ng generate library threeobject
# ng i -g three
# npm i --save-dev @types/three

# Build Angular project
# ---------------------
# cd /mnt/scc/src/
# ng build threeobject
# ng build scc

# Run Angular project from docker
# -------------------------------
# ng serve --host 0.0.0.0
# http://localhost:4201/

RUN npm install -g @angular/cli
RUN npm install -g three