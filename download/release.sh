#!/bin/bash

toolkitFolder="/carla-sdk"
versionFile="$toolkitFolder/version"
versionToDownload="1.0.0"
repoUrl="https://github.com/lucasFelixSilveira/carla"

function is_git_installed {
  if ! command -v git &> /dev/null; then
    echo "Git não está instalado."
    exit 1
  fi
}

function is_clang_installed {
  if ! command -v clang &> /dev/null; then
    echo "Clang não está instalado."
    clang_installed=false
  else
    clang_installed=true
  fi
}

function install_clang {
  echo "Instalando Clang..."
  sudo apt update
  sudo apt install llvm -y
}

function set_env_path {
  echo "Configurando o caminho no PATH..."
  echo "export PATH=\$PATH:$toolkitFolder" >> ~/.bashrc
  source ~/.bashrc
}

if [ -d "$toolkitFolder" ]; then
  if [ -f "$versionFile" ]; then
    currentVersion=$(cat "$versionFile")

    if [ "$currentVersion" == "$versionToDownload" ]; then
      echo "A versão $versionToDownload já está instalada."
    else
      read -p "A versão $versionToDownload não está instalada. Deseja atualizar para essa versão? (S/N): " userInput
      if [[ "$userInput" == "S" || "$userInput" == "s" ]]; then
        echo "Atualizando para a versão $versionToDownload..."
        git clone "$repoUrl" "$toolkitFolder"

        if [ "$clang_installed" == false ]; then
          install_clang
        fi

        set_env_path

        echo
        echo "Atualização concluída com sucesso!"
      else
        echo
        echo "A atualização foi cancelada."
      fi
    fi
  else
    echo "O arquivo 'version' não foi encontrado no diretório $toolkitFolder."
  fi
else
  git clone "$repoUrl" "$toolkitFolder"

  if [ "$clang_installed" == false ]; then
    install_clang
  fi

  set_env_path
  echo
  echo "Download concluído com sucesso!"
fi
