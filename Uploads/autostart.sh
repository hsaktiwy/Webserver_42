#!/bin/bash

# colors
RED='\033[0;31m'
NO_COLOR='\033[0m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
BGREEN='\033[1;32m'
YELLOW='\033[1;33m'
CYAN='\033[1;36m'

# ------------------------------- SPECIFY YOUR CONFIG HERE -------------------------------
THEME="dark"                                                          # "dark" or "light"
SYSTEM_APPS=(                                                         # specify the apps you want to open
    "Google Chrome"
    "Visual Studio Code"
    "Discord"
    "System Preferences"
    "Launchpad"
    )
USER_APPS_PATH="/Users/$(whoami)/Desktop/"                            # specify the path to your user apps
USER_APPS=(
    )                                                                 # specify the apps you want to open
WALLPAPER_PATH=""     # keep this empty if you don't want to change the wallpaper
# ---------------------------------------------------------------------------------------

open_apps(){
    echo -e "${YELLOW}> Opening apps...${NO_COLOR}"
    for app in "${SYSTEM_APPS[@]}"; do
        if [[ -d "/Applications/$app.app" ]]; then
            echo -e "${GREEN}✅ $app opened${NO_COLOR}"
            open -a "$app"
        elif [[ -d "/System/Applications/$app.app" ]]; then
            echo -e "${GREEN}✅ $app opened${NO_COLOR}"
            open -a "$app"
        else
            echo -e "${RED}❌ $app is not installed${NO_COLOR}"
        fi
    done

    for app in "${USER_APPS[@]}"; do
        if [[ -d "$USER_APPS_PATH$app.app" ]]; then
            echo -e "${GREEN}✅ $app opened${NO_COLOR}"
            open -a "$app"
        else
            echo -e "${RED}❌ $app is not installed${NO_COLOR}"
        fi
    done
    sleep 1
}

set_system_theme(){
    echo "❗ Please allow the permission to change the system theme"
    if [[ $1 == "dark" ]]; then
        echo -e "${YELLOW}> Setting system theme to dark mode...${NO_COLOR}"
        osascript -e 'tell application "System Events" to tell appearance preferences to set dark mode to true'
        echo -e "${GREEN}✅ System theme set${NO_COLOR}"
    elif [[ $1 == "light" ]]; then
        echo -e "${YELLOW}> Setting system theme to light mode...${NO_COLOR}"
        osascript -e 'tell application "System Events" to tell appearance preferences to set dark mode to false'
        echo -e "${GREEN}✅ System theme set${NO_COLOR}"
    else
        echo -e "${RED}❌ Invalid theme: $1${NO_COLOR}"
    fi
    sleep 1
}

set_wallpaper(){
    if [[ -z "$WALLPAPER_PATH" ]]; then
        return
    fi
    echo -e "${YELLOW}> Setting wallpaper...${NO_COLOR}"
    
    echo "❗ Please allow the permission to change the wallpaper"
    
    if [[ -f "$WALLPAPER_PATH" ]]; then
        osascript -e 'tell application "Finder" to set desktop picture to POSIX file "'"$WALLPAPER_PATH"'"'
        echo -e "${GREEN}✅ Wallpaper set${NO_COLOR}"
    else
        echo -e "${RED}❌ $WALLPAPER_PATH does not exist or is not a file${NO_COLOR}"
    fi
    sleep 1
}

clear_dock(){
    echo -e "${YELLOW}> Clearing dock...${NO_COLOR}"
    defaults write com.apple.dock persistent-apps -array > /dev/null 2>&1
    killall Dock > /dev/null 2>&1
    sleep 1
}

add_dock_apps(){
    echo -e "${YELLOW}> Adding apps to dock...${NO_COLOR}"

    for app in "${SYSTEM_APPS[@]}"; do
        if [[ -d "/Applications/$app.app" ]]; then
            app_path="/Applications/$app.app"
            echo -e "${GREEN}✅ $app added to dock${NO_COLOR}"
        elif [[ -d "/System/Applications/$app.app" ]]; then
            app_path="/System/Applications/$app.app"
        else
            echo -e "${RED}❌ $app is not installed${NO_COLOR}"
            continue
        fi
        defaults write com.apple.dock persistent-apps -array-add "<dict><key>tile-data</key><dict><key>file-data</key><dict><key>_CFURLString</key><string>$app_path</string><key>_CFURLStringType</key><integer>0</integer></dict></dict></dict>"
    done

    for app in "${USER_APPS[@]}"; do
        if [[ -d "$USER_APPS_PATH$app.app" ]]; then
            echo -e "${GREEN}✅ $app added to dock${NO_COLOR}"
            app_path="$USER_APPS_PATH$app.app"
            defaults write com.apple.dock persistent-apps -array-add "<dict><key>tile-data</key><dict><key>file-data</key><dict><key>_CFURLString</key><string>$app_path</string><key>_CFURLStringType</key><integer>0</integer></dict></dict></dict>"
        else
            echo -e "${RED}❌ $app is not installed${NO_COLOR}"
        fi
    done

    sleep 1
    killall Dock
}

function clean_glob {
	# don't do anything if argument count is zero (unmatched glob).
	if [ -z "$1" ]; then
		return 0
	fi
	/bin/rm -rf "$@" &>/dev/null
	return 0
}

function init_cclean {
    echo -e "\n${YELLOW}> Cleaning system...${NO_COLOR}"
    Storage=$(df -h "$HOME" | grep "$HOME" | awk '{print($4)}' | tr 'i' 'B')
    if [ "$Storage" == "0BB" ];
    then
	    Storage="0B"
    fi
    echo -e "${NO_COLOR}⏳ Available storage before cleaning: $Storage ${NO_COLOR}"
	shopt -s nullglob

	echo -ne "\033[38;5;208m"

	#42 Caches
	clean_glob "$HOME"/Library/*.42*
	clean_glob "$HOME"/*.42*
	clean_glob "$HOME"/.zcompdump*
	clean_glob "$HOME"/.cocoapods.42_cache_bak*

	#Trash
	clean_glob "$HOME"/.Trash/*

	#General Caches files
	#giving access rights on Homebrew caches, so the script can delete them
	/bin/chmod -R 777 "$HOME"/Library/Caches/Homebrew &>/dev/null
	clean_glob "$HOME"/Library/Caches/*
	clean_glob "$HOME"/Library/Application\ Support/Caches/*

	#Slack, VSCode, Discord and Chrome Caches
	clean_glob "$HOME"/Library/Application\ Support/Slack/Service\ Worker/CacheStorage/*
	clean_glob "$HOME"/Library/Application\ Support/Slack/Cache/*
	# clean_glob "$HOME"/Library/Application\ Support/discord/Cache/*
	# clean_glob "$HOME"/Library/Application\ Support/discord/Code\ Cache/js*
	# clean_glob "$HOME"/Library/Application\ Support/discord/Crashpad/completed/*
	clean_glob "$HOME"/Library/Application\ Support/Code/Cache/*
	clean_glob "$HOME"/Library/Application\ Support/Code/CachedData/*
	clean_glob "$HOME"/Library/Application\ Support/Code/Crashpad/completed/*
	clean_glob "$HOME"/Library/Application\ Support/Code/User/workspaceStorage/*
	# clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Profile\ [0-9]/Service\ Worker/CacheStorage/*
	# clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Default/Service\ Worker/CacheStorage/*
	# clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Profile\ [0-9]/Application\ Cache/*
	# clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Default/Application\ Cache/*
	# clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Crashpad/completed/*

	#.DS_Store files
	clean_glob "$HOME"/Desktop/**/*/.DS_Store

	#tmp downloaded files with browsers
	clean_glob "$HOME"/Library/Application\ Support/Chromium/Default/File\ System
	clean_glob "$HOME"/Library/Application\ Support/Chromium/Profile\ [0-9]/File\ System
	clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Default/File\ System
	clean_glob "$HOME"/Library/Application\ Support/Google/Chrome/Profile\ [0-9]/File\ System

	#things related to pool (piscine)
	clean_glob "$HOME"/Desktop/Piscine\ Rules\ *.mp4
	clean_glob "$HOME"/Desktop/PLAY_ME.webloc

    Storage=$(df -h "$HOME" | grep "$HOME" | awk '{print($4)}' | tr 'i' 'B')
    if [ "$Storage" == "0BB" ];
    then
    	Storage="0B"
    fi
    echo -e "${NO_COLOR}🚀 Available storage after cleaning: $Storage ${NO_COLOR}"
}

function initApp {
	echo -e "\033c"
    echo -e "${BLUE}--------------- 1337 Launcher -------------------${NO_COLOR}"
}

function exitApp {
    echo -e "\n${GREEN}✅ Finished!${NO_COLOR}"
    echo -e "${BLUE}-------------------------------------------------${NO_COLOR}"
    exit
}

function scroll_direction {
    # Change the scroll direction of the mouse
    defaults write -g com.apple.swipescrolldirection -bool false
}

function ff {
    # Add Launchpad to the Dock
    defaults write com.apple.dock persistent-apps -array-add '{"tile-type"="spacer-tile";}'
    defaults write com.apple.dock persistent-apps -array-add '{"tile-type"="directory-tile"; "tile-data"={"displayas"="folder"; "file-data"={"_CFURLString"="file:///Applications/Launchpad.app/"; "_CFURLStringType"=15;};};}'

    # Add System Preferences to the Dock
    defaults write com.apple.dock persistent-apps -array-add '{"tile-type"="spacer-tile";}'
    defaults write com.apple.dock persistent-apps -array-add '{"tile-type"="directory-tile"; "tile-data"={"displayas"="folder"; "file-data"={"_CFURLString"="file:///System/Library/PreferencePanes/"; "_CFURLStringType"=15;};};}'
}

initApp
init_cclean
set_system_theme $THEME
open_apps
# clear_dock
add_dock_apps
set_wallpaper
scroll_direction
ff
exitApp
