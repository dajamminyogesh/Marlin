#!/usr/bin/env bash
git remote add creatbot https://github.com/dajamminyogesh/OpenSourceCreatBotMarlinV2.git || git remote set-url creatbot https://github.com/dajamminyogesh/OpenSourceCreatBotMarlinV2.git
git fetch creatbot
git remote set-url creatbot https://server.creatbot.com/Gitea/CreatBot/OpenSourceCreatBotMarlinV2
git fetch creatbot
git remote set-url origin https://github.com/MarlinFirmware/Marlin.git
git fetch origin
git remote set-url origin git@github.com:dajamminyogesh/Marlin.git
git fetch origin
