package shelltest

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"strings"
	"testing"
	"time"
	"syscall"
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

	cmd := exec.Command("bash", "-c", "cd && pwd")
	if err != nil {
		t.Fatalf("Shell command \"cd\" failed: %v", err)
	}

	expectedOutput, err := cmd.Output()
	if err != nil {
		t.Fatalf("%v",err)
	}

	if ContainsOutput(output, strings.TrimSpace(string(expectedOutput))) {
		t.Log("cd test passed ✔️")
	} else {
        t.Errorf("Expected '%v' in output, got: %v", strings.TrimSpace(string(expectedOutput)),output)
	}
	
}

func TestCDDir(t *testing.T) {
	t.Log("...Testing cd dir command...") 

	RunShellCommand("../buildDir/oshell", "cd ~/Dokumente")
	output, err := RunShellCommand("../buildDir/oshell", "pwd")
	if err != nil {
        t.Fatalf("Shell command failed: %v", err)
	}
	
	cmd := exec.Command("bash", "-c" ,"cd ~/Dokumente")

	expectedOutput, err := cmd.Output()
	if err != nil {
		t.Fatalf("%v", err)
	}

	if ContainsOutput(output, string(expectedOutput)) {
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

	time.Sleep(100 * time.Millisecond)

	output, err := RunShellCommand("../buildDir/oshell", "kill spotify")
	if err != nil {
		cmd.Process.Kill() // cleanup
        t.Fatalf("Shell command failed: %v", err)
	}
	

    err = cmd.Process.Signal(syscall.Signal(0)) // Check if process still exists
	if ContainsOutput(output, "killed spotify") && err == nil{
		t.Log("kill by name test passed ✔️")
	} else {
		cmd.Process.Kill()
		t.Errorf("Expected 'killed spotify' in output, got: %v", output)
	}
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

	time.Sleep(100 * time.Millisecond)
	// get process ID 
	pid := cmd.Process.Pid

	var command string = fmt.Sprintf("kill %d", pid)
	output, err := RunShellCommand("../buildDir/oshell", command)
	if err != nil {
		cmd.Process.Kill()
		t.Fatalf("Shell command failed: %v", err)
	}

    err = cmd.Process.Signal(syscall.Signal(0)) // Check if process still exists
	var expectedOutput string = fmt.Sprint("killed " ,pid)
	if ContainsOutput(output, expectedOutput) && err == nil {
		t.Log("kill by PID test passed ✔️")
	} else {
		cmd.Process.Kill()
		t.Errorf("Expected '%s' in output, got : %v", expectedOutput, output)
	}
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
	err := cmd.Run()
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


	// Trims the whitespaces, and then replaces, the linebreaks with one space 
	expectedStr := strings.ReplaceAll(strings.TrimSpace(string(fileContent)), "\n", " ")
	gotStr := strings.ReplaceAll(strings.TrimSpace(string(lsFileContent)), "\n", " ")

	if ContainsOutput(strings.Split(expectedStr, "\n"), gotStr) {
		t.Log("output redirection test passed ✔️")
	} else {
		t.Errorf("Expected \n'%v' output, got: \n'%v'", strings.Split(expectedStr, "\n"), gotStr)
	}
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

	t.Log("...Testing ...")

	appendHere := "appendHere.txt"
	os.Create(appendHere)
	
	// FIX: this output is empty

	var command [4]string
	expectedOutput := "line1\nline2\nline3\nline4\n"

	// Build all commands
	for i := range command {
		command[i] = fmt.Sprintf("line%d >> %s", i+1, appendHere) // note: i+1 for line1, line2, etc.
	}

	// Execute all commands
	for i := range command {
	_, err := RunShellCommand("../buildDir/oshell", command[i])
		if err != nil {
			t.Fatalf("Shell command failed: %v", err)
		}
	}

	// Now read the file to check final result
	catOutput, err := RunShellCommand("../buildDir/oshell", fmt.Sprintf("cat %s", appendHere))
	if err != nil {
		t.Fatalf("Failed to read file: %v", err)
	}

	// Join the output lines and compare
	actualOutput := strings.Split(strings.Join(catOutput, "\n"),"\n")

	if ContainsOutput(actualOutput, expectedOutput) {
		t.Log("output redirection test passed ✔️")
	} else {
		t.Errorf("Expected:\n'%s'\nGot:\n'%s'", expectedOutput, actualOutput)
	}
}

// NOTE: not sure if this is supported
// func TestHereDocument(t *testing.T) {
// 	/*
// # Here document with variable expansion
// name="Alice"
// cat << EOF
// Hello $name
// Welcome to the system
// EOF
// */
//
// 	output , err := RunShellCommand("../buildDir/oshell", fmt.Sprintf("cat %s", appendHere))
// 	if err != nil {
// 		t.Fatalf("Failed to read file: %v", err)
// 	}
//
// 	if ContainsOutput(actualOutput, expectedOutput) {
// 		t.Log("output redirection test passed ✔️")
// 	} else {
// 		t.Errorf("Expected:\n'%s'\nGot:\n'%s'", expectedOutput, )
// 	}
// }

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

	const commAmount = 6
	var command [commAmount]string
	var expectedOutput [commAmount]string

	command[0] = "\"test123\" | grep \"test\" " // expect test123
	expectedOutput[0] = "test123\n"

	command[1] = "\"foo bar baz\" | wc -w " // expect 3
	expectedOutput[1] = "3\n"

	command[2] = "\"3\n1\n2\" | sort -n" // expect 1\n2\n3
	expectedOutput[2] = "1\n2\n3\n"

	command[3] = "\"\" | cat" // expect ""
	expectedOutput[3] = ""

	command[4] = "\"no match\" | grep \"xyz\" " // expect no output ""
	expectedOutput[4] = ""

	command[5] = "cat nonexistent.txt | wc -l " // expect error to stderr + 0 to stdout
	expectedOutput[5] = "0\n"  // wc -l will output 0 since it gets no input from failed cat
	
	var output [commAmount]string
	// Execute all commands
	for i := range output {
		// error here
		tmpOutput , err := RunShellCommand("../buildDir/oshell", command[i])
		if err != nil {
			t.Fatalf("Shell command failed: %v", err)
		}

		output[i] = tmpOutput[0]
		/*
		we cant use output[i], since this would a string, and we need []string, so by doing [:] we slice the whole 
		thing 
		*/
		if ContainsOutput(output[:], strings.TrimSpace(expectedOutput[i])) {
			t.Log("output redirection test passed ✔️")
		} else {
			t.Errorf("Expected:\n'%s'\nGot:\n'%s'", strings.TrimSpace(expectedOutput[i]), output[i])
		}
	}
}

// func TestMultiPiping (t *testing.T) {
/*
# Triple pipe - classic pipeline
ls -la | grep txt | wc -l

# 4-stage pipeline
ps aux | grep -v grep | awk '{print $2}' | head -5

# Network-style command (if available)
netstat -tuln | grep LISTEN | awk '{print $4}' | sort

# Pipes at wrong positions (should fail gracefully)
| ls -la
ls -la |
ls | | wc
ls |  | wc  # extra spaces
*/
// }
