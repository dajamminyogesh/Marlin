#!/usr/bin/env bash
set -eux
cd ..
if [[ ! -d OpenSourceCreatBotMarlinV2.git ]]; then
  git clone --mirror git@github.com:dajamminyogesh/OpenSourceCreatBotMarlinV2.git 
fi
cd OpenSourceCreatBotMarlinV2.git
git remote set-url origin git@github.com:dajamminyogesh/OpenSourceCreatBotMarlinV2.git
git fetch origin
git remote set-url origin https://server.creatbot.com/Gitea/CreatBot/OpenSourceCreatBotMarlinV2
git fetch origin
git remote set-url origin git@github.com:dajamminyogesh/OpenSourceCreatBotMarlinV2.git
git push --mirror
