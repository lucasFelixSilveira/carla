use serde_json::Value;

use colored::*;

pub fn assembly(data: Value, content: String, title: String) {
  let local: &Vec<Value> = 
    data.get("location").unwrap()
    .as_array().unwrap();

  let msg: &str = 
    data.get("message").unwrap()
    .as_str().unwrap();

  let buffer: &str = 
    data.get("buffer").unwrap()
    .as_str().unwrap();
  
  let line: u64 = local[0].as_u64().unwrap();
  let col:  u64 = local[1].as_u64().unwrap();

  println!("{} {} {}", 
    [ &line.to_string(), ":", &(col + 1).to_string(), ":" ].concat().bright_white().bold(),
    [title, ":".into()].concat().bright_red().bold(),
    msg.bright_white().bold()
  );

  let lines: Vec<&str> = content.lines().collect();
  let line_: &str = lines[(line as usize)-1];
  let mut line_content: String = line_.trim().to_string();
  let reduced: usize = line_.len() - line_content.len();

  if line_content.contains("--") {
    let (content, _) = line_content.split_once("--").unwrap();
    line_content = content.trim().to_string();
  }

  let preview: String = [&line.to_string(), " |"].concat();
  
  println!("{}{}{}{}", 
    "~".repeat(preview.len() + 1 - reduced).green(),
    "~".repeat(col as usize).green(),
    "v".repeat(buffer.len()).green(),
    "~".repeat(line_content.len() - col as usize - buffer.len()).green(),
  );

  println!("{} {}\n{} {} {}\n", 
    preview.purple().bold(), 
    line_content, 
    "└─ ".purple().bold(),
    "You can change de visibility like that:".bright_blue().bold(),
    if line_content.ends_with(") {") || line_content.ends_with(") {}") || line_content.ends_with(") {};") {
      format!("\n    {} {}{} {}",
        preview.green().bold(),  
        line_content[0..line_content.len() - {
          if line_content.ends_with(") {") { 1 } 
          else if line_content.ends_with(") {}") { 2 } 
          else if line_content.ends_with(") {};") { 3 } 
          else { line_content.len() }
        }].to_string(),
        "our".bright_purple().bold(),
        "-- Using The `our` keyword".green().bold()
      )
    } else { String::from("Unkown") }
  );
}