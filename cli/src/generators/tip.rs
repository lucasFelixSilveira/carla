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

  let can: &str = 
    data.get("can").unwrap()
    .as_str().unwrap();

  let tip: u64 = 
    data.get("tip").unwrap()
    .as_u64().unwrap();
  
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
  
  let uses = 
    data.get("uses");

  match uses {
    None => {}
    Some(data) => {

      let local: &Vec<Value> = 
        data.get("location").unwrap()
        .as_array().unwrap();
    
      let buffer: &str = 
        data.get("buffer").unwrap()
        .as_str().unwrap();
    
      let line: u64 = local[0].as_u64().unwrap();
      let col:  u64 = local[1].as_u64().unwrap();

      let line_: &str = lines[(line as usize)-1];
      let line_content: String = line_.trim().to_string();
      let preview: String = [&line.to_string(), " |"].concat();

      println!("{} {}", preview.purple().bold(), line_content);

      println!("{}{}{}{}", 
        "~".repeat(preview.len() + 1 - reduced).yellow(),
        "~".repeat(col as usize).yellow(),
        "^".repeat(buffer.len()).yellow(),
        "~".repeat(line_content.len() - col as usize - buffer.len()).yellow(),
      );

      println!("{} {} {} {}",
        ">".bright_blue().bold(), 
        [&line.to_string(), ":", &col.to_string()].concat().bright_white().bold(),
        "Note:".bright_blue().bold(),
        "The content was defined in".bright_green().bold()
      );
    },
  }

  println!("{}{}{}{}", 
    "~".repeat(preview.len() + 1 - reduced).green(),
    "~".repeat(col as usize).green(),
    "v".repeat(buffer.len()).green(),
    "~".repeat(line_content.len() - col as usize - buffer.len()).green(),
  );

  println!("{} {}\n{} {} \n    {} {}\n", 
    preview.purple().bold(), 
    line_content.clone(), 
    "└─ ".purple().bold(),
    ["You can ", can, " like that:"].concat().bright_blue().bold(),
    preview.green().bold(),
    parser(tip, msg, line_content, (line, col))
  );
}


fn parser(tip: u64, msg: &str, line_content: String, _: (u64, u64)) -> String {
  let mut cline:  String = line_content.clone();

  macro_rules! comment_remove {
    ($ctx:expr, $splitter:literal) => {
      if $ctx.contains($splitter) {
        let (n, _) = $ctx.split_once($splitter).unwrap();
        *$ctx = n.into();
      }
    };
  }

  fn remove_comments(ctx: &mut String) {
    comment_remove!(ctx, "--");
    comment_remove!(ctx, "-#");
    *ctx = ctx.trim().into();
  }

  const CHANGE_VISIBILITY: u64 = 0;
  match tip {

    CHANGE_VISIBILITY if msg.ends_with("private method") => {
      remove_comments(&mut cline);

      if cline.ends_with(") {") || cline.ends_with("){") {
        return format!("{}{}{{", 
          cline[0..(cline.len() - 1)].to_string().trim(), 
          " our ".bright_purple().bold()
        );
      }

      if cline.ends_with(") {}") || cline.ends_with("){}") {
        return format!("{}{}{{}}", 
          cline[0..(cline.len() - 2)].to_string().trim(), 
          " our ".bright_purple().bold()
        );
      }

      if cline.ends_with(") {};") || cline.ends_with("){};") {
        return format!("{}{}{{}};", 
          cline[0..(cline.len() - 3)].to_string().trim(), 
          " our ".bright_purple().bold()
        );
      }

      format!("Fail to parse the code shard. - You can change the visibility of a method using the `our` keyword")
    }

    _ => "Snippet not found.".into()
  }
}