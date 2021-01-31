#!/bin/bash

git remote add gitea ssh://git@git.nullgemm.fr:2999/nullgemm/$(NAME).git
git remote add github git@github.com:nullgemm/$(NAME).git
git remote remove origin
