#!/usr/bin/bash

tmux new-session -d -s dev

tmux rename-window -t dev:0 "NULL"

tmux new-window -t dev:1 -n "bash"
tmux new-window -t dev:2 -n "neovim"

tmux send-keys -t dev:2 "nvim ." C-m

tmux select-window -t dev:2
tmux attach-session -t dev
