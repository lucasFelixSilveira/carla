@echo off
setlocal

set "toolkitFolder=C:\carla-sdk"
set "binariesFolder=C:\carla-sdk\bin\windows"
set "versionFile=%toolkitFolder%\version"
set "versionToDownload=1.0.0"
set "repoUrl=https://github.com/lucasFelixSilveira/carla"

where git >nul 2>nul
if %errorlevel% neq 0 (
  echo Git nao esta instalado.
  exit /b
)

where clang >nul 2>nul
if %errorlevel% neq 0 (
  echo Clang nao esta instalado.
  set "clang_installed=false"
) else (
  set "clang_installed=true"
)

where gcc >nul 2>nul
if %errorlevel% neq 0 (
  echo Gcc nao esta instalado.
  set "gcc_installed=false"
) else (
  set "gcc_installed=true"
)


if "%clang_installed%"=="false" (
  echo Instalando Clang...
  choco install llvm -y
)

setx PATH "%PATH%;%toolkitFolder%"

if exist "%toolkitFolder%" (
  if exist "%versionFile%" (
    set /p currentVersion=<%versionFile%

    if "%currentVersion%"=="%versionToDownload%" (
      echo A versao %versionToDownload% ja esta instalada.
    ) else (
      set /p userInput= "A versao %versionToDownload% nao esta instalada. Deseja atualizar para essa versao? (S/N): "
      if /i "%userInput%"=="S" (
        echo Atualizando para a versao %versionToDownload%...
        git clone %repoUrl% %toolkitFolder%

        if "%clang_installed%"=="false" (
          choco install llvm -y
        )

        echo
        echo Atualizacao concluida com sucesso!
        pause
      ) else (
        echo
        echo A atualizacao foi cancelada.
        pause
      )
    )
  ) else (
    echo O arquivo 'version' nao foi encontrado no diretorio %toolkitFolder%.
    pause
  )
) else (
  git clone %repoUrl% %toolkitFolder%

  if "%clang_installed%"=="false" (
    choco install llvm -y
  )

  echo
  echo Download concluido com sucesso!
  pause
)

endlocal
