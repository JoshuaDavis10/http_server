#!/usr/bin/bash

tmux new-session -d -s xcb

tmux rename-window -t xcb:0 "NULL"

tmux new-window -t xcb:1 -n "bash"
tmux new-window -t xcb:2 -n "neovim"
tmux new-window -t xcb:3 -n "python"

tmux send-keys -t xcb:2 "nvim ." C-m
tmux send-keys -t xcb:3 "python3" C-m

tmux select-window -t xcb:2
tmux attach-session -t xcb
