# Build and run image
# -------------------
# docker build -t scc .
# docker run -p 4201:4200 scc
# docker container list
# docker stop <container id>
# docker system prune -f

FROM node:24-alpine

WORKDIR /usr/src/app

COPY . /usr/src/app

RUN npm install -g @angular/cli
RUN npm install three
RUN npm install

RUN ng version
RUN ng build threeobject
RUN ng build scc

CMD ["ng", "serve", "--host", "0.0.0.0"]
