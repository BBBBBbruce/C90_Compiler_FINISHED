int self_recursion(int i){
  if(i == 0) {
       return 5;
  }
  return self_recursion(i-1);
}
