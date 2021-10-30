 FROM gitpod/workspace-full
 
 RUN sudo add-apt-repository ppa:mongoose-os/mos && sudo apt-get update && sudo apt-get install -y mos
