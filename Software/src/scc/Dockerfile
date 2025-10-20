FROM node:22-alpine

# Build and run image
# -------------------
# docker build -t scc .
# docker run -p 4201:4200 scc
# See:https://dev.to/rodrigokamada/creating-and-running-an-angular-application-in-a-docker-container-40mk

WORKDIR /usr/src/app

COPY . /usr/src/app

RUN npm install -g @angular/cli
RUN npm install -g three
RUN npm install

RUN ng build threeobject
RUN ng build scc

CMD ["ng", "serve", "--host", "0.0.0.0"]
