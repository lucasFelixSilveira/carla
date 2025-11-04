#!/bin/bash
# =============================================================================
#  CARLA + MORGANA INSTALLER
#  • Detects current shell (zsh, bash, etc) via 'ps'
#  • Adds PATH to correct RC file (.bashrc OR .zshrc)
#  • Prevents duplicates (cleans old entries)
#  • Makes binary executable
#  • Full error handling + user prompts
# =============================================================================

set -euo pipefail
IFS=$'\n\t'

# ----------------------------------------------------------------------
#  HELPER: expand_path — safely expand ~
# ----------------------------------------------------------------------
expand_path() {
    local path="$1"
    eval echo "$path" 2>/dev/null || echo "$path"
}

# ----------------------------------------------------------------------
#  CORE: add_to_path — adds to PATH (session + correct RC file)
# ----------------------------------------------------------------------
add_to_path() {
    local raw_dir="$1"
    local dir
    dir=$(expand_path "$raw_dir")
    dir=$(realpath "$dir" 2>/dev/null || echo "$dir")

    # === DETECT CURRENT SHELL (RELIABLE) ===
    local current_shell
    current_shell=$(ps -p $$ -o comm= 2>/dev/null || basename "$SHELL" 2>/dev/null || echo "sh")
    local shell_rc

    case "$current_shell" in
        *bash)  shell_rc="$HOME/.bashrc" ;;
        *zsh)   shell_rc="$HOME/.zshrc" ;;
        *)      shell_rc="$HOME/.profile" ;;
    esac

    if [ ! -d "$dir" ]; then
        echo "ERROR: Directory does not exist: $dir" >&2
        return 1
    fi

    # === CURRENT SESSION ===
    if ! echo "$PATH" | grep -Eq "(^|:)$dir(:|$)"; then
        export PATH="$dir:$PATH"
        echo "Added to current PATH: $dir"
    else
        echo "Already in current PATH: $dir"
    fi

    # === PERMANENT: deduplicate + add ===
    [ -f "$shell_rc" ] || touch "$shell_rc"

    local escaped_dir=$(printf '%s\n' "$dir" | sed 's/[\$]/\\&/g')
    local export_line="export PATH=\"$escaped_dir:\$PATH\""

    # Remove old entries
    sed -i "\|export PATH=.*$escaped_dir|d" "$shell_rc" 2>/dev/null || true
    sed -i "/# Added by Carla installer/,+2d" "$shell_rc" 2>/dev/null || true

    # Add clean block
    {
        echo ""
        echo "# Added by Carla installer — $(date '+%Y-%m-%d %H:%M')"
        echo "$export_line"
    } >> "$shell_rc"

    echo "Permanently added to: $shell_rc"
}

# ----------------------------------------------------------------------
#  INSTALL MORGANA
# ----------------------------------------------------------------------
install_morgana() {
    local base_dir="$HOME/.morgana"
    local repo_dir="$base_dir/morgana"
    local bin_dir="$repo_dir/bin"

    echo "Installing Morgana → $base_dir"
    mkdir -p "$base_dir"
    pushd "$base_dir" >/dev/null

    [ -d "morgana" ] && rm -rf morgana

    echo "Cloning Morgana..."
    git clone https://github.com/lucasFelixSilveira/morgana.git || {
        echo "ERROR: Failed to clone Morgana" >&2
        popd >/dev/null
        return 1
    }

    pushd morgana >/dev/null
    [ ! -f "build.sh" ] && { echo "ERROR: build.sh missing!" >&2; popd >/dev/null; popd >/dev/null; return 1; }

    echo "Building Morgana..."
    chmod +x build.sh
    ./build.sh || { echo "ERROR: Build failed!" >&2; popd >/dev/null; popd >/dev/null; return 1; }

    # Make binary executable
    [ -f "$bin_dir/morgana" ] && chmod +x "$bin_dir/morgana"

    popd >/dev/null
    add_to_path "$bin_dir"
    popd >/dev/null
    echo "Morgana installed and added to PATH"
}

# ----------------------------------------------------------------------
#  INSTALL CARLA
# ----------------------------------------------------------------------
install_carla() {
    local base_dir="$HOME/.carla"
    local repo_dir="$base_dir/carla"
    local build_dir="$repo_dir/build"

    echo "Installing Carla → $base_dir"
    mkdir -p "$base_dir"
    pushd "$base_dir" >/dev/null

    [ -d "carla" ] && rm -rf carla

    echo "Cloning Carla..."
    git clone https://github.com/lucasFelixSilveira/carla.git || {
        echo "ERROR: Failed to clone Carla" >&2
        popd >/dev/null
        return 1
    }

    pushd carla >/dev/null
    [ ! -f "build.sh" ] && { echo "ERROR: build.sh missing!" >&2; popd >/dev/null; popd >/dev/null; return 1; }

    echo "Building Carla..."
    chmod +x build.sh
    ./build.sh || { echo "ERROR: Build failed!" >&2; popd >/dev/null; popd >/dev/null; return 1; }

    # CRITICAL: Make carla executable
    if [ -f "$build_dir/carla" ]; then
        chmod +x "$build_dir/carla"
        echo "Made carla executable: $build_dir/carla"
    else
        echo "WARNING: carla binary not found in $build_dir!" >&2
    fi

    popd >/dev/null
    add_to_path "$build_dir"
    popd >/dev/null
    echo "Carla installed and added to PATH"
}

# ----------------------------------------------------------------------
#  PROMPT: Yes/No
# ----------------------------------------------------------------------
prompt_yn() {
    local msg="$1"
    local default="${2:-Y}"
    local choice
    while true; do
        echo -n "$msg [$([ "$default" = "Y" ] && echo "Y/n" || echo "y/N")]: "
        read choice
        choice="${choice:-$default}"
        case "${choice,,}" in
            y|yes) return 0 ;;
            n|no)  return 1 ;;
            *) echo "Please type 'y' or 'n'" ;;
        esac
    done
}

# ----------------------------------------------------------------------
#  MAIN LOGIC
# ----------------------------------------------------------------------
if command -v carla >/dev/null 2>&1; then
    echo "Carla is already installed."
    if prompt_yn "Update it?" "Y"; then
        echo "Updating Carla..."
        rm -rf "$HOME/.carla"
        install_carla

        if prompt_yn "Update Morgana too?" "Y"; then
            echo "Updating Morgana..."
            rm -rf "$HOME/.morgana"
            install_morgana
        fi
    else
        echo "Update skipped."
    fi
else
    echo "Carla is not installed."
    if prompt_yn "Install it?" "Y"; then
        if prompt_yn "Carla needs Morgana. Install it too?" "Y"; then
            install_morgana
        fi
        install_carla
    else
        echo "Installation canceled."
        exit 0
    fi
fi

# ----------------------------------------------------------------------
#  FINAL INSTRUCTIONS
# ----------------------------------------------------------------------
echo ""
echo "SUCCESS: Installation complete!"
echo ""
echo "To use 'carla' and 'morgana' NOW, run:"
echo "   source ~/.bashrc    # if using bash"
echo "   source ~/.zshrc     # if using zsh"
echo ""
echo "Or restart your terminal."
echo ""
echo "Verify:"
echo "   which carla"
echo "   which morgana"
echo "   carla --version"
echo ""
