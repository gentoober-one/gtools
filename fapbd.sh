#!/bin/bash

#     ____            __        __
#    / __/___ _____  / /_  ____/ /
#   / /_/ __ `/ __ \/ __ \/ __  / 
#  / __/ /_/ / /_/ / /_/ / /_/ /  
# /_/  \__,_/ .___/_.___/\__,_/   
#          /_/                    
#-----------------------------------------------------------------------------------------------
#  bulk downloader for fapello/fapeza
#-----------------------------------------------------------------------------------------------
#         by gentoober (2023)
#-----------------------------------------------------------------------------------------------
# Usage: ./fapbd <URL>
# e.g.: ./fapbd https://fapello.com/content/d/a/dainty-wilder-66/1000/dainty-wilder-66_0045.jpg
#-----------------------------------------------------------------------------------------------

# colors
GREEN="\033[1;32m"
RED="\033[1;31m"
CYAN="\033[1;36m"
MAGENTA="\033[1;35m"
YELLOW="\033[1;33m"
PURPLE="\033[1;34m"
DARK_GRAY="\033[1;30m"
LIGHT_BLUE="\033[1;94m"
NC="\033[0m"

# download function
download_images() {
    local base_url="$1"
    local start="$2"
    local max_attempts=10

    local i="$start"
    local attempt=0
    while [ $attempt -lt $max_attempts ]; do
        local filename="${content}_$(printf "%04d" $i).jpg"
        local url="$base_url/$filename"

        # use wget --spider to check if the file exists
        if wget --spider "$url" 2>/dev/null; then
            wget -N -c "$url"
            echo -e "${GREEN}Downloaded picture $i${NC}"
        else
            echo -e "${YELLOW}Image not found. Retrying... (Attempt $((attempt+1)))${NC}"
            i=$((i + 1))
            attempt=$((attempt + 1))
            continue
        fi

        i=$((i + 1))
        attempt=0 
    done

    if [ $attempt -eq $max_attempts ]; then
        echo -e "${RED}Reached maximum retry limit. No more images to download.${NC}"
    fi
}

if [ "$#" -ne 1 ]; then
    echo -e "${CYAN}Uso: $0 <URL>${NC}"
    exit 1
fi

url="$1"
content=$(echo -e "$url" | cut -d '/' -f 7)
dest_dir="$content"
url_base="$(dirname "$url")"

if [ ! -d "$dest_dir" ]; then
    mkdir -p "$dest_dir"
fi

cd "$dest_dir"

# extract the image number from the URL and start downloading from that number
image_number=$(echo -e "$url" | grep -oP '(?<=_)\d+(?=\.)' | sed 's/^0*//')
download_images "$url_base" "$image_number"

cd ..

echo -e "${CYAN}Pictures downloaded!${NC}"
