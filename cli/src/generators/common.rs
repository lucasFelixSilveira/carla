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
  let line_content: String = line_.trim().to_string();
  let reduced: usize = line_.len() - line_content.len();
  let preview: String = [&line.to_string(), " |"].concat();
  println!("{} {}", preview.purple().bold(), line_content);

  println!("{}{}{}{}", 
    "~".repeat(preview.len() + 1 - reduced).green(),
    "~".repeat(col as usize).green(),
    "^".repeat(buffer.len()).green(),
    "~".repeat(line_content.len() - col as usize - buffer.len()).green(),
  );
  
}