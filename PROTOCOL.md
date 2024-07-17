# 2024, Carla Protocol - Silveira, Lucas

As we observe the current landscape of programming, there appears to be a discernible decline in intellectual rigor. We've transitioned away from the era of luminaries such as Ada Lovelace, Dennis Ritchie, Ken Thompson, and Linus Torvalds, finding ourselves amidst a milieu dominated by average Python or JavaScript users. Have you too noticed this intellectual regression within our field? It seems that rational thought has yielded to a more pedestrian approach, reminiscent of a less sophisticated era. Join us, fellow Carla Protocol users, in advocating for technology for technology's sake.

# What is "technology for technology"?
The concept of pursuing something for its inherent value can be distilled into a simple idea: learning for the sake of learning—an ethos famously championed by Bertrand Russell in his seminal work "The Problems of Philosophy."

# Protocol rigors:

To adhere to the Carla Protocol, a programming language must meet stringent standards, including:

1. Adoption of a modern syntax, exemplified by languages such as Rust, Go, or Kotlin.
2. Avoidance of magic implementations.
3. Inclusion of a robust standard library for mathematics.
4. Maintenance of a logical syntax that prioritizes clarity over abstraction, regardless of its modernity.
5. Provision of names with historical significance, honoring influential figures or pivotal moments in history, or even, a person important to you.
6. Aim for a rational approach in programming without unnecessary complexity.

# Glossary
- What are Magic implementations?

Magic implementations refer to coding practices that condense complex operations into overly concise code blocks, sacrificing clarity and maintainability for brevity.
## ✨ Magic 
```zig
const std = @import("std");

fn main() !void {
  const fs = std.fs;
  const allocator = std.heap.page_allocator;

  const file = try fs.cwd().openFile("exemple.txt", .{ .read = true, .write = false });
  defer file.close();

  var lineCount: u32 = 0;
  var buffer = try allocator.alloc(u8, 4096);
  defer allocator.free(buffer);

  while (try file.read(buffer, 4096)) |buffer, bytesRead| {
    for (buffer[0 .. bytesRead]) |byte| {
      if (byte == '\n') {
        lineCount += 1;
      }
    }
  }

  std.debug.print("
Number of lines in the file: {}\n", .{lineCount});
}
```
## ✅ Normal
```c
#include <stdio.h>

int main() {
  FILE *file = fopen("exemple.txt", "r");
  if (file == NULL) {
    perror("Error opening the file");
    return 1;
  }

  int lineCount = 0;
  char buffer[4096];
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    lineCount++;
  }

  fclose(file);
  printf("Number of lines in the file: %d\n", lineCount);

  return 0;
}
```

##
### Note on the Carla Protocol:

The Carla Protocol is constantly developing to guarantee the integrity and efficiency of the adopted standards. New additions to the rigors may occur as programming technologies and practices evolve. This document is considered untouchable by the open source community within the project, ensuring its consistency and applicability.