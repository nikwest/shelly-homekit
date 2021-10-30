 FROM gitpod/workspace-full
 
 RUN add-apt-repository -y ppa:mongoose-os/mos && apt-get update && apt-get install -y mos
