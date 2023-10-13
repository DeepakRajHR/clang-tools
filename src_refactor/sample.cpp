
struct ST {
  int x;
};

void setValue(struct ST &v) {
  v.x = 1;
}

int main() {
  struct ST s;
  setValue(s);
  return 0;
}
