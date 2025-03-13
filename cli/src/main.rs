use std::path::PathBuf;
use std::{env, fs};
use std::process::{Command, Stdio};
use std::iter::{Enumerate, Skip};
use std::slice::Iter;

use colored::*;

#[inline]
fn cli_error(msg: &str) {
  println!("{}: {msg}", "[CLI ERROR]".red())
}

fn main() {
  println!("teste");
  let runner: &str;
  if cfg!(target_os = "windows") {
    runner = "cmd"
  } else {
    runner = "sh"
  }

  type Arguments = Vec<String>; 
  let arguments: Vec<String> = env::args().skip(1).collect::<Arguments>();
  if let Some(doing) = arguments.get(0) {
    let path: PathBuf  = env::current_dir().unwrap();
    let cwd: &str = path.to_str().unwrap();

    match doing.as_str() {
      "compile" => {
        let mut main: &String = &String::from("main.crl");
        let mut output: String = String::new();

        if arguments.len() > 1 {
          let maybe_flags: Enumerate<Skip<Iter<'_, String>>> = arguments.iter().skip(1).enumerate();
          let flags: Vec<&String> = maybe_flags.clone().into_iter().map(|x| x.1).collect();
          let mut skip: bool = false;
          for (index, argument) in maybe_flags {
            if skip { skip = !skip; continue; }

            match argument.as_str() {
              "--main" | "-m" => {
                main = &flags[index + 1];
                skip = true;
                continue
              }
              "--output" | "-o" => {
                output = flags[index + 1].clone();
                skip = true;
                continue
              }
              _ => cli_error(
                &format!("The `{argument}` flag is not found")
              )
            }
          }
        }

        if let Err(_) = fs::read_to_string([cwd, std::path::MAIN_SEPARATOR_STR, main].concat()) {
          cli_error(
            &format!("Your project is missing the default main file. Try using the flag to change the main file reference. `--main`")
          );
          return;
        }

        print!("Generating the LLVM code");

        let mut result = 
          Command::new(runner)
            .args(if cfg!(target_os = "windows") {
              vec!["/C", "carlac", main]
            } else {
              vec!["-c", "carlac", main]
            })
            .stdout(Stdio::null())
            .stderr(Stdio::null())
            .spawn();

        match result {
          Ok(mut child) => {
            let success = "Success".green();
            print!(" - {success}\n└─ Emitting the LLVM Objetct");
            
            let _ = child.wait(); 

            let out_folder: String = [cwd, std::path::MAIN_SEPARATOR_STR, "target", std::path::MAIN_SEPARATOR_STR, "out"].concat();
            let ir_file: String = [&out_folder, std::path::MAIN_SEPARATOR_STR, "ir.ll"].concat();
            let exe_file: String = [&out_folder, std::path::MAIN_SEPARATOR_STR, "out"].concat();

            let mut compilation_call: Vec<&str> = Vec::new(); 

            /* Std flags */
            compilation_call.push(if cfg!(target_os = "windows") { "/C" } else { "-C"});
            compilation_call.push("clang");
            compilation_call.push(&ir_file);

            /* Ouput flags */
            compilation_call.push("-o");
            compilation_call.push(if! output.is_empty() { &output } else { &exe_file });

            /* Optimization flags */
            compilation_call.push("-O3");
            compilation_call.push("-march=native");
            compilation_call.push("-mtune=native");
            compilation_call.push("-ftree-slp-vectorize");
            compilation_call.push("-fvisibility=hidden");
            compilation_call.push("-ftree-vectorize");
            compilation_call.push("-fno-math-errno");
            compilation_call.push("-fomit-frame-pointer");
            compilation_call.push("-ffast-math");
            compilation_call.push("-fopenmp");
            compilation_call.push("-fvectorize");
            compilation_call.push("-fprofile-arcs");
            compilation_call.push("-ftest-coverage");
            compilation_call.push("-mavx512f");
            compilation_call.push("-mavx2");
            compilation_call.push("-Ofast");
            compilation_call.push("-funroll-loops");

            result = 
              Command::new(runner)
                .args(if cfg!(target_os = "windows") {
                  compilation_call
                } else {
                  compilation_call
                })
                .stdout(Stdio::null())
                .stderr(Stdio::null())
                .spawn();

            match result {
              Ok(mut child) => {
                let _ = child.wait(); 
                println!(" - {success}\nYour project has been compiled!");
              }
              Err(e) => eprintln!("Fail to compile! Error on call the clang: {}", e)
            }

          }
          Err(e) => eprintln!("Fail to compile! Error on call the process: {}", e),
        }

      }
      "help" => {
        println!(
          concat!(
            "carla [action] [flags](optional)\n",
            " actions:\n",
            "\t- compile: (Possible flags) --main (alias -m)",
            "\t                          | --output (alias -o)"
          )
        );
      }
      _ => cli_error(
        &format!("`{doing}` it's not a valid action. You can find a valid action using 'help' with the action")
      )
    }

  } else { cli_error("You need to inform the action you want") }
}