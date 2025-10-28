use expectrl::{
    spawn,
    repl::ReplSession,
    Expect,
    process::Termios,
};
use std::io::{Read, Result, Write};
use std::process::Command;
use std::fs::File;
use regex::Regex;

use nix;

fn spawn_shell() -> Result<ReplSession<expectrl::session::OsSession>> {
    let mut p = spawn("../buildDir/oshell")?;
    p.set_echo(true)?;

    let mut shell = ReplSession::new(p, String::from("oshell> "));
    shell.set_echo(true);
    shell.set_quit_command("exit");
    shell.expect_prompt()?; // wait until prompt appears

    Ok(shell)
}
fn exec_debug(shell: &mut ReplSession<expectrl::session::OsSession>, cmd: &str) -> String {
    println!("\n=== EXECUTING: {} ===", cmd);
    
    // Try with a longer timeout for this specific command
    let buf = match shell.execute(cmd) {
        Ok(b) => {
            println!("✓ Command succeeded");
            b
        },
        Err(e) => {
            println!("✗ Command failed/timed out: {:?}", e);
            
            // Try to read whatever is available
            println!("Attempting to read available buffer...");
            panic!("Timeout on: {}", cmd);
        }
    };
    
    // Show raw bytes
    println!("Length:    {:?}", buf.len());
    println!("Raw bytes:    {:?}", buf);
    println!("Raw string:   {:?}", String::from_utf8_lossy(&buf));

    let mut s = String::from_utf8_lossy(&buf).into_owned();
    
    s = s.trim_start_matches("\u{1b}[?2004h").to_string();
    s = s.trim_end_matches("\u{1b}[?2004l\r").to_string();
    s = s.replace("\r\n", "\n"); // Normalize line endings
    
    println!("After cleanup: {:?}", s);
    println!("=== END ===\n");
    
    s
}

fn exec(shell: &mut ReplSession<expectrl::session::OsSession>, cmd: &str) -> String {
    let buf = shell.execute(cmd).expect("command failed");
    let s = String::from_utf8_lossy(&buf).into_owned();
    
    // Remove ANSI escape sequences more carefully
    let re = Regex::new(r"\x1b\[[^\x1b]*?[a-zA-Z]").unwrap();
    let cleaned = re.replace_all(&s, "");
    
    cleaned.replace("\r\n", "\n").replace("\r", "")
}

// fn exec(shell: &mut ReplSession<expectrl::session::OsSession>, cmd: &str) -> String {
//     let buf = shell.execute(cmd).expect("command failed");
//     let mut s = String::from_utf8_lossy(&buf).into_owned();
//     // clean up possible ANSI sequences if needed
//     s = s.replace("\r\n\u{1b}[?2004l\r", "").replace("\r\n\u{1b}[?2004h", "");
//     s
// }

#[test]
fn test_echo_hello() {
    let mut shell = spawn_shell().expect("failed to start shell");
    let output = exec(&mut shell, "echo hello");
    assert!(output.contains("hello"));
}

#[test]
fn test_cd_home() {
    let tmp_home = Command::new("bash")
        .arg("-c")
        .arg("cd ~ && pwd")
        .output()
        .unwrap();
    let home = String::from_utf8_lossy(&tmp_home.stdout).trim().to_string();

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

fn spawn_process() -> u32 {
    let process = Command::new("spotify")
        .spawn()
        .expect("failed to spawn process");

        process.id()
}

// NOTE: Kill_by_* always panics here. Why? the process should be killed
fn is_alive(pid: u32) -> bool {
    let t = std::path::Path::new(&format!("/proc/{}", pid)).exists();
    println!("{}", t);
    return t;
}

#[test]
fn test_kill_by_name() {
    let (mut child, pid) = spawn_process();

    println!("Spawned process PID: {}", pid);

    std::thread::sleep(std::time::Duration::from_secs(1));

    assert!(is_alive(pid), "Process should be alive initially");

    let mut shell = spawn_shell().unwrap();
    let t_proc_name = &format!("ps -p {} -o comm=", pid);
    let proc_name = exec(&mut shell, t_proc_name).trim().to_string();

    println!("killing: {}", proc_name);

    let command = &format!("kill {}", proc_name);
    exec(&mut shell, command).trim().to_string();

    std::thread::sleep(std::time::Duration::from_secs(2));

    let alive = is_alive(pid);
    assert!(!alive, "Process {} should be killed", pid);

}

#[test] 
fn test_kill_by_pid() {
    let pid = spawn_process();
    println!("Spawned process PID: {}", pid);

    std::thread::sleep(std::time::Duration::from_secs(1));
    assert!(is_alive(pid), "Process should be alive");

    println!("before kill - /proc/{} exists: {}", pid, std::path::Path::new(&format!("/proc/{}", pid)).exists());
    println!("before kill - /proc/{}/stat contents:", pid);

    // Try reading the stat file directly
    if let Ok(content) = std::fs::read_to_string(format!("/proc/{}/stat", pid)) {
        println!("{}", content);
    }

   if let Ok(content) = std::fs::read_to_string(format!("/proc/{}/stat", pid)) {
        let parts: Vec<&str> = content.split_whitespace().collect();
        let ppid = parts.get(3).unwrap_or(&"?");
        println!("Parent PID of {}: {}", pid, ppid);
        println!("parent proc - /proc/{} : {}", ppid, std::path::Path::new(&format!("/proc/{}", ppid)).exists());
        if let Ok(content) = std::fs::read_to_string(format!("/proc/{}/stat", ppid)) {
            println!("{}", content);
        }
    }

    let mut shell = spawn_shell().unwrap();

    println!("killing: {}", pid);
    let result = exec(&mut shell, &format!("kill {}", pid)).trim().to_string();
    println!("Kill command output: {}", result);

    let ps = exec(&mut shell, "ps aux | grep 8070").trim().to_string();
    println!("ps: {}", ps);


println!("Before kill - /proc/{} exists: {}", pid, std::path::Path::new(&format!("/proc/{}", pid)).exists());

let mut shell = spawn_shell().unwrap();
println!("killing: {}", pid);
exec(&mut shell, &format!("kill {}", pid)).trim().to_string();

std::thread::sleep(std::time::Duration::from_millis(500));

println!("After kill - /proc/{} exists: {}", pid, std::path::Path::new(&format!("/proc/{}", pid)).exists());
println!("After kill - /proc/{}/stat contents:", pid);

// Try reading the stat file directly
if let Ok(content) = std::fs::read_to_string(format!("/proc/{}/stat", pid)) {
    println!("{}", content);
}


    std::thread::sleep(std::time::Duration::from_secs(5));
    let alive = is_alive(pid);
    assert!(!alive, "Process {} should be killed", pid);
}

#[test]
fn test_append() -> Result<()> {
    let filename = "appendHere.txt";
    File::create(filename)?;

    let expected_output= "line1\nline2\nline3\nline4\n";


    let mut shell = spawn_shell().unwrap();
    let commands: [String; 4] = std::array::from_fn(|i| {
        format!("echo \"line{}\" >> {}", i+1, filename)
    });
    for command in &commands {
        exec(&mut shell, command).trim().to_string();
    }

    let mut open_file = File::open(filename)?;
    let mut contents = String::new();
    open_file.read_to_string(&mut contents)?;
    assert_eq!(expected_output, contents);

    Ok(())
}


#[test]
fn test_single_pipe() -> Result<()>{
    let mut commands: [&str; 3] = Default::default();

    commands[0] = "echo \"test123\" | grep \"test\""; // expect test123

    commands[1] = "echo \"foo bar baz\" | wc -w"; // expect 3

    // TODO: this times out, why?
    commands[2] = "echo -e \"3\\n1\\n2\" | sort -n"; // expect 1\n2\n3
    // commands[3] = "echo \"\" | cat"; // expect ""
    /* 
     * NOTE:
     *   left: "�Ĥj�U"  -> OSHELL
     *   right: ""      -> BASH
     * */

    let mut shell = spawn_shell().unwrap();
    for command in &commands {

    let oshell_out = exec_debug(&mut shell, command).replace("\r\n", "\n").trim().to_string();

        let output = Command::new("bash")
        .arg("-c")
        .arg(command)
        .output()
        .expect("something went wrong");

        let output_str= String::from_utf8_lossy(&output.stdout).trim().to_string();
        assert_eq!(oshell_out, output_str);
    }
    Ok(())
}


#[test]
fn test_multi_pipe() {
let mut commands: [&str; 3] = Default::default();

    commands[0] = "ls -la | grep txt | wc -l";

    commands[1] = "ps aux | grep -v grep | awk '{print $2}' | head -5"; 

    commands[2] = "netstat -tuln | grep LISTEN | awk '{print $4}' | sort"; 

    let mut shell = spawn_shell().unwrap();
    for command in &commands {
    let oshell_out = exec(&mut shell, command).replace("\r\n", "\n").trim().to_string();

        let output = Command::new("bash")
        .arg("-c")
        .arg(command)
        .output()
        .expect("something went wrong");

        let output_str= String::from_utf8_lossy(&output.stdout).trim().to_string();
        assert_eq!(oshell_out, output_str);
    }
}

#[test]
fn test_command_chaining() {
    let mut commands: [&str; 3] = Default::default();

    commands[0] = "echo one && echo two && echo three";
    commands[1] = "echo start && false && echo end";
    commands[2] = "false && echo should_not_run && echo nope";

    let mut shell = spawn_shell().unwrap();
    for command in &commands{
        let shell_out = exec(&mut shell, command).replace("\r\n", "\n").trim().to_string();
        let output = Command::new("bash")
        .arg("-c")
        .arg(command)
        .output()
        .expect("something went wrong");

        let output_str= String::from_utf8_lossy(&output.stdout).trim().to_string();
        assert_eq!(shell_out, output_str);
    }

}
