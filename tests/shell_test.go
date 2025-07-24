package shelltest

import (
	"os/exec"
	"testing"
	"bufio"
	"strings"
)

/*
    t.Error("Something went wrong")     // Mark test as failed, continue
    t.Fatal("Critical failure")        // Mark test as failed, stop immediately
    t.Log("Debug info")                // Print info (only shown if test fails or -v flag)
    t.Skip("Skipping this test")       // Skip this test
    
    // Check conditions
    if result != expected {
        t.Errorf("Expected %v, got %v", expected, result)
    }
*/


/*
Inbuilt
1. help ✔️
2. cd dir, cd (home) ✅
3. env ie echo $HOST ✅
4. kill
	5. input redirection
	6. output redirection
	7. piping
	
*/

// NOTE: for testing, there does not need to be a main function, go is smart enough
// to call every funtion that starts with Test

func RunShellCommand(shellPath, command string) ([]string, error) {
    cmd := exec.Command(shellPath)
    
    stdin, err := cmd.StdinPipe()
    if err != nil {
        return nil, err
    }
 
	cmd.Stderr = cmd.Stdout
    stdout, err := cmd.StdoutPipe()
    if err != nil {
        return nil, err
    }
    
    err = cmd.Start()
    if err != nil {
        return nil, err
    }
    
    // Send command
    stdin.Write([]byte(command + "\n"))
    stdin.Write([]byte("exit\n"))
    stdin.Close()
    
    // Read output
    var output []string
    scanner := bufio.NewScanner(stdout)
    for scanner.Scan() {
        output = append(output, scanner.Text())
    }
    

    cmd.Wait()
    return output, nil
}

func ContainsOutput(output []string, expected string) bool {
	// this is prob for each?
    for _, line := range output {
        if strings.Contains(strings.ToLower(line), strings.ToLower(expected)) {
            return true
        }
    }
    return false
}

// test functions have to start with Test
func TestEchoCommand(t *testing.T) {
	t.Log("...Testing echo command...")

    output, err := RunShellCommand("../buildDir/oshell", "echo hello")
    if err != nil {
        t.Fatalf("Shell command failed: %v", err)
    }
    
    if ContainsOutput(output, "hello") {
		t.Log("echo test passed ✅")
    } else {
        t.Errorf("Expected 'hello' in output, got: %v", output)
	}
}


// oshell cant to && so chaining commands does not work
func TestCDHome(t *testing.T) {
	t.Log("...Testing cd command...")

	RunShellCommand("../buildDir/oshell", "cd")
	output, err := RunShellCommand("../buildDir/oshell", "pwd")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}

	if ContainsOutput(output, "/home/ole") {
		t.Log("cd test passed ✔️")
	} else {
        t.Errorf("Expected '/home/ole' in output, got: %v", output)
	}

}

func TestCDDir(t *testing.T) {
	t.Log("...Testing cd dir command...") 

	RunShellCommand("../buildDir/oshell", "cd ~/Dokumente/")
	output, err := RunShellCommand("../buildDir/oshell", "pwd")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(output, "/home/ole/Dokumente/") {
		t.Log("cd dir test passed ✔️")
	} else {
        t.Errorf("Expected '/home/ole/Dokumente/' in output, got: %v", output)
	}
}



func TestENV(t *testing.T) {
	t.Log("...Testing env output...")
	
	t.Log("testing echo $USER...")
	outputUSER, err := RunShellCommand("../buildDir/oshell", "echo $USER")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(outputUSER, "ole") {
		t.Log("echo $USER test passed ✔️")
	} else {
        t.Errorf("Expected 'ole' in output, got: %v", outputUSER)
	}

// --------------------

	t.Log("testing echo $HOME...")
	outputHOME, err := RunShellCommand("../buildDir/oshell", "echo $HOME")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(outputHOME, "/home/ole") {
		t.Log("echo $HOME test passed ✔️")
	} else {
        t.Errorf("Expected '/home/ole/' in output, got: %v", outputHOME)
	}
	
// --------------------

	t.Log("testing echo $PWD...")
	outputPWD, err := RunShellCommand("../buildDir/oshell", "echo $PWD")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(outputPWD, "/home/ole/Dokumente/Projekte/c/oshell/buildDir") {
		t.Log("echo $PWD test passed ✔️")
	} else {
        t.Errorf("Expected '/home/ole/Dokumente/Projekte/c/oshell/buildDir' in output, got: %v", outputPWD)
	}

// --------------------

	t.Log("testing echo $DISPLAY...")
	outputDISPLAY, err := RunShellCommand("../buildDir/oshell", "echo $DISPLAY")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(outputDISPLAY, ":0") {
		t.Log("echo $DISPLAY test passed ✔️")
	} else {
		t.Errorf("Expected ':0' in output, got: %v", outputDISPLAY)
	}

// --------------------
}


// FIX: This tecnically fails, because the output does not match, but spotify is killed
func TestKillByNAME(t *testing.T) {
	t.Log("testing kill by name...")
	t.Log("if this fails make sure spotify is open, since this test tries to kill spotify")
	output, err := RunShellCommand("../buildDir/oshell", "kill spotify")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(output, "killed process spotify") {
		t.Log("kill by name test passed ✔️")
	} else {
		t.Errorf("Expected 'kill process spotify' in output, got: %v", output)
	}
}
