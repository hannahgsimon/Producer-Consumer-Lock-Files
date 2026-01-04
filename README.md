# Producer–Consumer Synchronization Using IPC (Lock Files)

## 🧭 Overview
This project implements the classic **producer–consumer** coordination problem using **Linux IPC-style synchronization using filesystem-based lock files**. The producer reads values from an input file one-by-one and writes each value into a single-item shared buffer location, while the consumer reads the value and appends it to an output file. Random sleeps simulate asynchronous execution.

The synchronization logic prevents:
- **Race conditions**
- **Overwriting / data loss** (producer too fast)
- **Duplicate consumption** (consumer too fast)

## ⚙️ How It Works
The design uses a **single-item buffer** (a file) plus **two lock files**:

- `produced.lock` – indicates a new value has been produced and is ready to be consumed
- `consumed.lock` – indicates the previous value has been consumed and the producer may write the next one

### High-level workflow

**Producer**
1. Sleep for a small random time
2. Read next value from `input.txt`
3. Wait until the previous value is marked “consumed”
4. Write value to `buffer.txt`
5. Signal “produced”

**Consumer**
1. Sleep for a small random time
2. Wait until a value is marked “produced”
3. Read value from `buffer.txt`
4. Append value to `output.txt`
5. Signal “consumed”

After processing N items (e.g., 10), `output.txt` should match `input.txt`.

## 📁 Files

- `producer.cxx` – producer process
- `consumer.cxx` – consumer process
- `input.txt` – input values (one per line)
- `buffer.txt` – single-item shared buffer (created at runtime)
- `output.txt` – consumer output (created at runtime)
- `produced.lock`, `consumed.lock` – lock files (created/removed at runtime)

---

## 🛠️ Build

On Linux / WSL:

```bash
g++ -O2 -Wall producer.cxx -o producer
g++ -O2 -Wall consumer.cxx -o consumer
```
> If you’re using pure C instead of C++, compile with `gcc` and rename files to `.c` (or adjust the build command accordingly).

## ▶️ Run

In one terminal:

```bash
./producer
```

In a second terminal:
```bash
./consumer
```

## ✅ Verify Output
After both finish:
```bash
diff -u input.txt output.txt
```
No output from diff means the files match.

## 🧹 Notes / Tips
If you re-run the program, delete generated files first:
```bash
rm -f buffer.txt output.txt produced.lock consumed.lock
```
Random sleep calls simulate different execution speeds and help expose synchronization bugs.

---

## 📜 License

This project is licensed under the MIT License. See the LICENSE file for details.

---

## 👤 Author
Hannah G. Simon
