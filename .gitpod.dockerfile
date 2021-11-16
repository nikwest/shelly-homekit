 FROM gitpod/workspace-full
 
 RUN sudo add-apt-repository -y ppa:mongoose-os/mos && sudo apt-get update && sudo apt-get install -y mos
