use expectrl::{
    spawn,
    repl::ReplSession,
    Expect,
    process::Termios,
};
use std::io::Result;
use std::process::Command;


fn spawn_shell() -> Result<ReplSession<expectrl::session::OsSession>> {
    println!("cwd = {:?}", std::env::current_dir());
    let mut p = spawn("../buildDir/oshell")?;
    p.set_echo(true)?;

    let mut shell = ReplSession::new(p, String::from("oshell> "));
    shell.set_echo(true);
    shell.set_quit_command("exit");
    shell.expect_prompt()?; // wait until prompt appears

    Ok(shell)
}

fn exec(shell: &mut ReplSession<expectrl::session::OsSession>, cmd: &str) -> String {
    let buf = shell.execute(cmd).expect("command failed");
    let mut s = String::from_utf8_lossy(&buf).into_owned();
    // clean up possible ANSI sequences if needed
    s = s.replace("\r\n\u{1b}[?2004l\r", "").replace("\r\n\u{1b}[?2004h", "");
    s
}

#[test]
fn test_echo_hello() {
    let mut shell = spawn_shell().expect("failed to start shell");
    let output = exec(&mut shell, "echo hello");
    assert!(output.contains("hello"));
}

#[test]
fn test_cd_home() {
    let home = Command::new("bash")
        .arg("-c")
        .arg("cd ~ && pwd")
        .output()
        .unwrap();
    let home = String::from_utf8_lossy(&home.stdout).trim().to_string();

    // Now run the same cd in your shell and check
    let mut shell = spawn_shell().unwrap();
    exec(&mut shell, &format!("cd {}", home));
    let output = exec(&mut shell, "pwd").trim().to_string();

    assert_eq!(output, home);
}

#[test]
fn test_cd_documents() {
    let documents_bash= Command::new("bash")
        .arg("-c")
        .arg("cd ~/Dokumente && pwd")
        .output()
        .unwrap();
    let documents_bash= String::from_utf8_lossy(&documents_bash.stdout).trim().to_string();


    let mut shell = spawn_shell().unwrap();
    let output = exec(&mut shell, "cd ~/Dokumente && pwd").trim().to_string();

    assert_eq!(output, documents_bash);
}

#[test]
fn test_env_var() {
    let mut shell = spawn_shell().unwrap();
    let username = std::env::var("USER").unwrap();
    let out_user = exec(&mut shell, "echo $USER").trim().to_string();
    assert_eq!(out_user, username);

    let pwd= std::env::var("PWD").unwrap();
    let out_pwd = exec(&mut shell, "echo $PWD").trim().to_string();
    assert_eq!(pwd, out_pwd);

    let display= std::env::var("DISPLAY").unwrap();
    let out_display= exec(&mut shell, "echo $DISPLAY").trim().to_string();
    assert_eq!(display, out_display);
}

// NOTE: This is way better then what I had 
fn spawn_process() -> u32 {
    let output = Command::new("bash")
        .arg("-c")
        .arg("sleep 60 & echo $!")
        .output()
        .expect("failed to spawn process");

    let pid_str = String::from_utf8_lossy(&output.stdout);
    pid_str.trim().parse::<u32>().expect("invalid PID")
}

fn is_alive(pid: u32) -> bool {
    let status = Command::new("bash")
        .arg("-c")
        .arg(format!("kill -0 {}", pid))
        .status()
        .unwrap();
    status.success() // `kill -0` returns 0 if process exists
}

fn kill_with_shell(pid: u32) {
    let mut shell = spawn("../../buildDir/oshell").unwrap();
    shell.send_line(&format!("kill {}", pid)).unwrap();
}

// FIX: This take a long time and the fails.
#[test]
fn test_kill_by_name() {
    let pid = spawn_process();
    println!("Spawned process PID: {}", pid);

    kill_with_shell(pid);

    let alive = is_alive(pid);
    assert!(!alive, "Process {} should be killed", pid);
    
}
