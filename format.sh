#!/bin/bash

astyle -i --style=ansi src/*
astyle -i --style=ansi include/*
astyle -i --style=ansi include/internal/*
