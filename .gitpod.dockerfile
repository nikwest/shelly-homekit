 FROM gitpod/workspace-full
 
 RUN add-apt-repository ppa:mongoose-os/mos && apt-get update && apt-get install -y mos
