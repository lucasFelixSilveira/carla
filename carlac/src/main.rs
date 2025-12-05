const VERSION: &'static str = "ALPHA";

use clap::Parser;

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Cli {
    /// Nome do usuário
    #[arg(short, long)]
    version: Option<String>,
}

fn main() {
    let args = Cli::parse();

    match args.version {
        Some(bin) => {
            let mut version: &str = VERSION;
            println!("The current version of {} is {}!", bin, version);
        }
        None => {}
    }
}
