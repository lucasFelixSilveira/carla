#!/bin/bash

set -euo pipefail
IFS=$'\n\t'

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Log file
LOG_FILE="${HOME}/.carla_installer.log"
INSTALL_TIMESTAMP=$(date '+%Y%m%d_%H%M%S')

# ----------------------------------------------------------------------
#  LOGGING FUNCTIONS
# ----------------------------------------------------------------------
log() {
    local level="$1"
    local message="$2"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')

    echo -e "${level}${message}${NC}" >&2
    echo "[$timestamp] $level$message" >> "$LOG_FILE"
}

info() { log "${BLUE}" "INFO: $1"; }
success() { log "${GREEN}" "SUCCESS: $1"; }
warning() { log "${YELLOW}" "WARNING: $1"; }
error() { log "${RED}" "ERROR: $1"; }

# ----------------------------------------------------------------------
#  ERROR HANDLING AND CLEANUP
# ----------------------------------------------------------------------
cleanup() {
    if [ $? -ne 0 ]; then
        error "Installation failed! Check log: $LOG_FILE"
    fi
}

trap cleanup EXIT

rollback_install() {
    local component="$1"
    warning "Performing rollback for $component..."

    case "$component" in
        "carla")
            if [ -d "$HOME/.carla" ]; then
                rm -rf "$HOME/.carla"
                info "Removed carla installation"
            fi
            ;;
        "morgana")
            if [ -d "$HOME/.morgana" ]; then
                rm -rf "$HOME/.morgana"
                info "Removed morgana installation"
            fi
            ;;
    esac
}

# ----------------------------------------------------------------------
#  VALIDATION FUNCTIONS
# ----------------------------------------------------------------------
check_commands() {
    local commands=("git" "gcc" "g++" "make" "cmake" "pkg-config")
    local missing=()

    for cmd in "${commands[@]}"; do
        if ! command -v "$cmd" >/dev/null 2>&1; then
            missing+=("$cmd")
        fi
    done

    if [ ${#missing[@]} -gt 0 ]; then
        error "Missing required commands: ${missing[*]}"
        info "Please install them with your package manager:"
        echo "  Ubuntu/Debian: sudo apt install ${missing[*]}"
        echo "  Fedora: sudo dnf install ${missing[*]}"
        echo "  Arch: sudo pacman -S ${missing[*]}"
        return 1
    fi

    success "All required commands are available"
}

check_libraries() {
    local libraries=("libcurl4-openssl-dev" "libssl-dev" "libxml2-dev" "libzip-dev")
    local missing=()

    for lib in "${libraries[@]}"; do
        if ! pkg-config --exists "${lib%-dev}" 2>/dev/null; then
            missing+=("$lib")
        fi
    done

    if [ ${#missing[@]} -gt 0 ]; then
        warning "Some development libraries might be missing: ${missing[*]}"
        info "You may need to install them if build fails:"
        echo "  sudo apt install ${missing[*]}"
    else
        success "All required libraries are available"
    fi
}

check_disk_space() {
    local required_mb=500
    local available_mb

    available_mb=$(df "$HOME" | awk 'NR==2 {print $4}')
    available_mb=$((available_mb / 1024))

    if [ "$available_mb" -lt "$required_mb" ]; then
        error "Insufficient disk space. Required: ${required_mb}MB, Available: ${available_mb}MB"
        return 1
    fi

    success "Disk space check passed: ${available_mb}MB available"
}

# ----------------------------------------------------------------------
#  SHELL DETECTION AND PATH MANAGEMENT
# ----------------------------------------------------------------------
detect_shell() {
    local current_shell
    echo $(basename "$SHELL")
}

get_shell_rc() {
    local shell="$1"
    case "$shell" in
        *bash)  echo "$HOME/.bashrc" ;;
        *zsh)   echo "$HOME/.zshrc" ;;
        *)      echo "$HOME/.profile" ;;
    esac
}

backup_rc_file() {
    local rc_file="$1"
    if [ -f "$rc_file" ]; then
        local backup="${rc_file}.backup.${INSTALL_TIMESTAMP}"
        cp "$rc_file" "$backup"
        success "Backed up $rc_file to $backup"
    fi
}

# ----------------------------------------------------------------------
#  PATH MANAGEMENT - ROBUST AND SAFE
# ----------------------------------------------------------------------
add_to_path() {
    local dir="$1"
    local current_shell=$(detect_shell)
    local shell_rc=$(get_shell_rc "$current_shell")

    info "Adding $dir to PATH for $current_shell (RC: $shell_rc)"

    if [ ! -d "$dir" ]; then
        error "Directory does not exist: $dir"
        return 1
    fi

    # Backup RC file
    backup_rc_file "$shell_rc"

    # Create RC file if it doesn't exist
    [ -f "$shell_rc" ] || touch "$shell_rc"

    # Remove old entries from this installer
    sed -i.bak "/# Added by Carla installer/d" "$shell_rc" 2>/dev/null || true
    sed -i.bak "|export PATH=.*$dir|d" "$shell_rc" 2>/dev/null || true

    # Add new entry with proper escaping
    local escaped_dir=$(printf '%s\n' "$dir" | sed 's/[\\.*^$[]/\\&/g')

    {
        echo ""
        echo "# Added by Carla installer - $INSTALL_TIMESTAMP"
        echo "export PATH=\"${dir}:\$PATH\""
    } >> "$shell_rc"

    # Add to current session
    export PATH="${dir}:$PATH"

    success "Added $dir to PATH (session and $shell_rc)"
}

# ----------------------------------------------------------------------
#  INSTALLATION FUNCTIONS
# ----------------------------------------------------------------------
install_morgana() {
    info "Starting Morgana installation..."

    local base_dir="$HOME/.morgana"
    local repo_dir="$base_dir/morgana"
    local bin_dir="$repo_dir/bin"

    # Clean previous installation
    if [ -d "$base_dir" ]; then
        warning "Removing previous Morgana installation..."
        rm -rf "$base_dir"
    fi

    mkdir -p "$base_dir"
    pushd "$base_dir" >/dev/null

    info "Cloning Morgana repository..."
    if ! git clone --depth 1 https://github.com/lucasFelixSilveira/morgana.git 2>>"$LOG_FILE"; then
        error "Failed to clone Morgana repository"
        popd >/dev/null
        return 1
    fi

    pushd morgana >/dev/null

    if [ ! -f "build.sh" ]; then
        error "build.sh not found in Morgana repository"
        popd >/dev/null
        popd >/dev/null
        return 1
    fi

    info "Building Morgana..."
    chmod +x build.sh

    if ! ./build.sh >> "$LOG_FILE" 2>&1; then
        error "Morgana build failed - check log: $LOG_FILE"
        popd >/dev/null
        popd >/dev/null
        rollback_install "morgana"
        return 1
    fi

    # Verify binary was created
    if [ ! -f "$bin_dir/morgana" ]; then
        error "Morgana binary not found after build"
        popd >/dev/null
        popd >/dev/null
        rollback_install "morgana"
        return 1
    fi

    chmod +x "$bin_dir/morgana"

    popd >/dev/null
    popd >/dev/null

    # Add to PATH
    add_to_path "$bin_dir"

    # Verify installation
    if command -v morgana >/dev/null 2>&1; then
        success "Morgana installed successfully"
        return 0
    else
        error "Morgana installation verification failed"
        rollback_install "morgana"
        return 1
    fi
}

install_carla() {
    info "Starting Carla installation..."

    local base_dir="$HOME/.carla"
    local repo_dir="$base_dir/carla"
    local build_dir="$repo_dir/build"

    # Clean previous installation
    if [ -d "$base_dir" ]; then
        warning "Removing previous Carla installation..."
        rm -rf "$base_dir"
    fi

    mkdir -p "$base_dir"
    pushd "$base_dir" >/dev/null

    info "Cloning Carla repository..."
    if ! git clone --depth 1 https://github.com/lucasFelixSilveira/carla.git 2>>"$LOG_FILE"; then
        error "Failed to clone Carla repository"
        popd >/dev/null
        return 1
    fi

    pushd carla >/dev/null

    if [ ! -f "build.sh" ]; then
        error "build.sh not found in Carla repository"
        popd >/dev/null
        popd >/dev/null
        return 1
    fi

    info "Building Carla..."
    chmod +x build.sh

    if ! ./build.sh >> "$LOG_FILE" 2>&1; then
        error "Carla build failed - check log: $LOG_FILE"
        popd >/dev/null
        popd >/dev/null
        rollback_install "carla"
        return 1
    fi

    # Verify binary was created
    if [ ! -f "$build_dir/carla" ]; then
        error "Carla binary not found after build"
        popd >/dev/null
        popd >/dev/null
        rollback_install "carla"
        return 1
    fi

    chmod +x "$build_dir/carla"

    popd >/dev/null
    popd >/dev/null

    # Add to PATH
    add_to_path "$build_dir"

    # Verify installation
    if command -v carla >/dev/null 2>&1; then
        success "Carla installed successfully"
        return 0
    else
        error "Carla installation verification failed"
        rollback_install "carla"
        return 1
    fi
}

# ----------------------------------------------------------------------
#  UPDATE FUNCTIONS
# ----------------------------------------------------------------------
check_updates() {
    info "Checking for updates..."

    local has_updates=false

    # Check Carla
    if [ -d "$HOME/.carla/carla" ]; then
        pushd "$HOME/.carla/carla" >/dev/null
        git fetch origin 2>>"$LOG_FILE"
        local behind_count=$(git rev-list HEAD..origin/main --count 2>/dev/null || echo "0")
        if [ "$behind_count" -gt 0 ]; then
            info "Carla has $behind_count new commits available"
            has_updates=true
        fi
        popd >/dev/null
    fi

    # Check Morgana
    if [ -d "$HOME/.morgana/morgana" ]; then
        pushd "$HOME/.morgana/morgana" >/dev/null
        git fetch origin 2>>"$LOG_FILE"
        local behind_count=$(git rev-list HEAD..origin/main --count 2>/dev/null || echo "0")
        if [ "$behind_count" -gt 0 ]; then
            info "Morgana has $behind_count new commits available"
            has_updates=true
        fi
        popd >/dev/null
    fi

    if [ "$has_updates" = true ]; then
        return 0  # Updates available
    else
        info "Both Carla and Morgana are up to date"
        return 1  # No updates
    fi
}

update_installation() {
    info "Updating installations..."

    if [ -d "$HOME/.morgana" ]; then
        if ! install_morgana; then
            error "Failed to update Morgana"
            return 1
        fi
    fi

    if [ -d "$HOME/.carla" ]; then
        if ! install_carla; then
            error "Failed to update Carla"
            return 1
        fi
    fi

    success "Update completed successfully"
}

# ----------------------------------------------------------------------
#  USER PROMPTS
# ----------------------------------------------------------------------
prompt_yn() {
    local msg="$1"
    local default="${2:-Y}"
    local choice
    local prompt

    if [ "$default" = "Y" ]; then
        prompt="[Y/n]"
    else
        prompt="[y/N]"
    fi

    while true; do
        echo -n "$msg $prompt: "
        read -r choice
        choice="${choice:-$default}"

        case "${choice,,}" in
            y|yes) return 0 ;;
            n|no)  return 1 ;;
            *) echo "Please answer 'y' or 'n'" ;;
        esac
    done
}

# ----------------------------------------------------------------------
#  MAIN INSTALLATION LOGIC
# ----------------------------------------------------------------------
main() {
    echo -e "${BLUE}"
    cat << "EOF"
    ___          _                     _   __  __
   / __|__ _ _ _| |__ _   __ _ _ _  __| | |  \/  |___ _ _ __ _ __ _ _ _  __ _
  | (__/ _` | '_| / _` | / _` | ' \/ _` | | |\/| / _ \ '_/ _` / _` | ' \/ _` |
   \___\__,_|_| |_\__,_| \__,_|_||_\__,_| |_|  |_\___/_| \__, \__,_|_||_\__,_|
EOF
    echo -e "${NC}"

    info "Starting installation process..."
    info "Log file: $LOG_FILE"

    # System checks
    info "Performing system checks..."
    check_commands
    check_libraries
    check_disk_space

    local current_shell=$(detect_shell)
    info "Detected shell: $current_shell"

    # Check if already installed
    local carla_installed=$(command -v carla >/dev/null 2>&1 && echo true || echo false)
    local morgana_installed=$(command -v morgana >/dev/null 2>&1 && echo true || echo false)

    if [ "$carla_installed" = true ] || [ "$morgana_installed" = true ]; then
        info "Existing installation detected"

        if check_updates; then
            if prompt_yn "Updates available. Install them?" "Y"; then
                if ! update_installation; then
                    error "Update failed"
                    exit 1
                fi
            else
                info "Update skipped by user"
            fi
        else
            info "Software is up to date"

            if prompt_yn "Reinstall anyway?" "N"; then
                if ! update_installation; then
                    error "Reinstallation failed"
                    exit 1
                fi
            else
                info "Reinstallation skipped"
            fi
        fi
    else
        info "Fresh installation"

        if prompt_yn "Install Carla and Morgana?" "Y"; then
            info "Installing Morgana (required for Carla)..."
            if ! install_morgana; then
                error "Morgana installation failed - cannot continue"
                exit 1
            fi

            info "Installing Carla..."
            if ! install_carla; then
                error "Carla installation failed"
                # Don't rollback Morgana as it might be useful standalone
                exit 1
            fi

            success "Installation completed successfully!"
        else
            info "Installation cancelled by user"
            exit 0
        fi
    fi

    # Final verification
    info "Performing final verification..."

    local verification_passed=true

    if command -v morgana >/dev/null 2>&1; then
        success "✓ Morgana is available in PATH"
    else
        error "✗ Morgana not found in PATH"
        verification_passed=false
    fi

    if command -v carla >/dev/null 2>&1; then
        success "✓ Carla is available in PATH"
    else
        error "✗ Carla not found in PATH"
        verification_passed=false
    fi

    if [ "$verification_passed" = false ]; then
        error "Installation verification failed!"
        info "Try running: source $(get_shell_rc "$(detect_shell)")"
        exit 1
    fi

    # Success message
    echo
    success "🎉 Installation completed successfully!"
    echo
    info "Next steps:"
    echo "  1. Run: source $(get_shell_rc "$(detect_shell)")"
    echo "  2. Or simply restart your terminal"
    echo "  3. Verify with: which carla && which morgana"
    echo
    info "Log file: $LOG_FILE"
    echo
}

if [ "$EUID" -eq 0 ]; then
    warning "This script should NOT be run as root/sudo!"
    warning "It will install software for the current user only."

    if prompt_yn "Continue as root anyway?" "N"; then
        warning "Installing as root user - this is not recommended"
    else
        info "Please run as normal user:"
        echo "  ./$(basename "$0")"
        exit 1
    fi
fi

touch "$LOG_FILE"
echo "=== CARLA INSTALLER LOG - $(date) ===" > "$LOG_FILE"

main "$@"
