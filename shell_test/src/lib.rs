use std::error::Error as OtherError;

#[test]
fn test_echo() -> Result<(), Box<dyn Error>> {
    let mut prcs = expectrl::spawn("../../buildDir/oshell")?;
    prcs.send_line("echo \"hello\"").unwrap();
    prcs.expect("hello")?;
    Ok(())
}

