package shelltest

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"strings"
	"testing"
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
4. kill (byname) ✅
4.5 kill byPID ✅
5. input redirection ✅
6. output redirection ✅
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
 
	stdout, err := cmd.StdoutPipe()
    if err != nil {
        return nil, err
    }
 
	
	// this makes sure that stdout and stderr, both point to the same location so 
	// they are basically merged together, here we make stderr point to stdout
	cmd.Stderr = cmd.Stdout

    err = cmd.Start()
    if err != nil {
        return nil, err
    }
    
    // Send command
    stdin.Write([]byte(command + "\n"))
    // stdin.Write([]byte("exit\n"))
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

    output, err := RunShellCommand("../buildDir/oshell", "echo hello\n")
    if err != nil {
        t.Fatalf("Shell command failed: %v", err)
    }
    
    if ContainsOutput(output, "hello") {
		t.Log("echo test passed ✔️")
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

	// TODO: Output here is kinda correct, prints right path, but adds "Exiting shell"
	t.Log("testing echo $PWD...")
	outputPWD, err := RunShellCommand("../buildDir/oshell", "echo $PWD ")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(outputPWD, "/home/ole/Dokumente/Projekte/c/oshell/tests") {
		t.Log("echo $PWD test passed ✔️")
	} else {
        t.Errorf("Expected '/home/ole/Dokumente/Projekte/c/oshell/buildDir/tests' in output, got: %v", outputPWD)
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


func TestKillByNAME(t *testing.T) {
	t.Log("testing kill by name...")
	// t.Log("if this fails make sure spotify is open, since this test tries to kill spotify")
	
	var process string = "spotify"
	cmd := exec.Command(process)

	err := cmd.Start()
	if err != nil {
		t.Fatalf("Could not start process 'spotify'")
	}



	output, err := RunShellCommand("../buildDir/oshell", "kill spotify")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	if ContainsOutput(output, "killed spotify") {
		t.Log("kill by name test passed ✔️")
	} else {
		t.Errorf("Expected 'killed spotify' in output, got: %v", output)
	}
	cmd.Wait()
}


// fuzz for killByPID? https://pkg.go.dev/testing#hdr-Fuzzing
func TestKillByPID(t *testing.T) {
	t.Log("...Testing kill by PID...")

	// how can we "generate a random PID"

	// we start process
	var process string = "spotify"
	cmd := exec.Command(process)

	err := cmd.Start()
	if err != nil {
		t.Fatalf("Could not start process 'spotify'")
	}

	// get process ID 
	pid := cmd.Process.Pid

	var command string = fmt.Sprintf("kill %d", pid)
	output, err := RunShellCommand("../buildDir/oshell", command)
	if err != nil {
		t.Fatalf("Shell command failed: %v", err)
	}

	var expectedOutput string = fmt.Sprint("killed " ,pid)
	if ContainsOutput(output, expectedOutput) {
		t.Log("kill by PID test passed ✔️")
	} else {
		t.Errorf("Expected '%s' in output, got : %v", expectedOutput, output)
	}
	cmd.Wait()
}


// input doesnt match output??
func TestInputRedirection(t *testing.T) {
	t.Log("...Testing input redirection...")

	// we need a file to redirect from, and content in it

	input := "oshell is a simple but cool project "
	err := os.WriteFile("string.txt", []byte(input), 0666)
	if err != nil {
		t.Fatalf("%v", err)
	}

	output, err := RunShellCommand("../buildDir/oshell", "cat < string.txt")

	if err != nil {
		t.Fatalf("Shell command failed: %v", err)
	} 
	if ContainsOutput(output, input) {
		t.Log("input redirection test passed ✔️")
	} else {
		t.Errorf("Expected '%v' in output, got: %v", input,output)
	}
}

// b is empty
func TestOutputRedirection (t *testing.T) {
	t.Log("...Testing output redirection...")

	lsFilename := "ls.txt"
	os.Create(lsFilename)
	os.Create("originalLs.txt")
	cmd := exec.Command("bash", "-c", "ls > originalLs.txt")
	err := cmd.Start()
	if err != nil {
		t.Fatalf("%v", err)
	}
	// read from file, to get output to combine
	fileContent, err:= os.ReadFile("originalLs.txt")
	if err != nil {
		t.Fatalf("%v", err)
	}

	var command = fmt.Sprintf("ls > %s", lsFilename)
	RunShellCommand("../buildDir/oshell", command)

	lsFileContent, err := os.ReadFile(lsFilename)
	if err != nil {
		t.Fatalf("%v", err)
	}

	if err != nil {
		t.Fatalf("Shell command failed: %v", err)
	} 
	if ContainsOutput(strings.Split(string(lsFileContent), "\n"), string(fileContent)) {
		t.Log("output redirection test passed ✔️")
	} else {
		t.Errorf("Expected \n'%s' output, got: \n%v", strings.Split(string(lsFileContent), "\n"), string(fileContent))
	}
	cmd.Wait()
}


// NOTE: this should be supported
func TestHereAppend(t *testing.T) {
	/*
echo "line 1" >> test.log
echo "line 2" >> test.log
echo "line 3" >> test.log

# Append command output
date >> logfile.txt
*/

}

// NOTE: not sure if this is supported
func TestHereDocument(t *testing.T) {
	/*
# Here document with variable expansion
name="Alice"
cat << EOF
Hello $name
Welcome to the system
EOF
*/
}

func TestSinglePiping (t *testing.T) {
/*
echo "test123" | grep "test"     # Should output: test123
echo "foo bar baz" | wc -w       # Should output: 3
echo "3\n1\n2" | sort -n         # Should output: 1\n2\n3

echo "" | cat                    # Empty input
echo "no match" | grep "xyz"     # No output (grep returns nothing)
yes | head -5                    # Infinite input stream
cat nonexistent.txt | wc -l      # Error handling
*/


}
