use serde_json::Value;

use colored::*;

pub fn assembly(data: Value, content: String) {
  let local: &Vec<Value> = 
    data.get("location").unwrap()
    .as_array().unwrap();

  let msg: &str = 
    data.get("message").unwrap()
    .as_str().unwrap();
  
  let line: u64 = local[0].as_u64().unwrap();
  let col:  u64 = local[1].as_u64().unwrap();

  println!("{} {} {}", 
    [ &line.to_string(), ":", &(col + 1).to_string(), ":" ].concat().bright_white().bold(),
    "Syntax type error:".bright_red().bold(),
    msg.bright_white().bold()
  );

  let lines: Vec<&str> = content.lines().collect();
  let line_content: &str =  lines[(line as usize)-1].trim();
  let preview: String = [&line.to_string(), " |"].concat();
  println!("{} {}", preview.purple().bold(), line_content);

  println!("{}{}{}{}", 
    "~".repeat(preview.len() + 1).green(),
    "~".repeat(col as usize).green(),
    "^".green(),
    "~".repeat(line_content.len() - col as usize - 1).green(),
  );
  
}