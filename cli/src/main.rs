use std::path::PathBuf;
use std::{env, fs, thread, time};
use std::process::{Command, Stdio};
use std::iter::{Enumerate, Skip};
use std::slice::Iter;
use std::sync::{Arc, Mutex};

#[inline]
fn cli_error(msg: &str) {
  println!("[CLI ERROR]: {msg}")
}

fn main() {
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

        let stop_flag = Arc::new(Mutex::new(false));  

        let stop_flag_clone = Arc::clone(&stop_flag);
        let task = thread::spawn(move || {
          let positions: Vec<char> = vec!['|', '/', '-', '\\', '|', '/', '-', '\\'];
          let qpos: usize = positions.len();
          let mut c: usize = 0; 
          loop {
            thread::sleep(time::Duration::from_millis(50));
            if *stop_flag_clone.lock().unwrap() {
              break;
            }

            let position: char = positions[c];
            if c == qpos - 1 { c = 0; } else { c += 1; }

            print!("{position} Compiling...\r");
          }
        });

        let result = 
          Command::new(runner)
            .args(if cfg!(target_os = "windows") {
                vec!["/C", "carlac", "main"]
            } else {
                vec!["-c", "carlac main"]
            })
            .stdout(Stdio::null())
            .stderr(Stdio::null())
            .spawn();

        match result {
          Ok(mut child) => {
            let _ = child.wait(); 

            let mut flag = stop_flag.lock().unwrap();
            *flag = true;

            println!("Your project has been compiled!")
          }
          Err(e) => eprintln!("Fail to compile! Error on call the process: {}", e),
        }

        let _ = task.join();

      }
      "help" => {
        println!(
          concat!(
            "carla [action] [flags](optional)\n",
            " actions:\n",
            "\t- compile: (Possible flags) --main (alias -m)"
          )
        );
      }
      _ => cli_error(
        &format!("`{doing}` it's not a valid action. You can find a valid action using 'help' with the action")
      )
    }

  } else { cli_error("You need to inform the action you want") }
}