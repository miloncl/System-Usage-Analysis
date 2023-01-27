# Feedback for Milon: 

0/. Good job on creating the code for the transition probs/matrices as well as accuracy. Pls see the below feedbacks for reasons behind my adjustment in the code

1/. 

```
def get_conditional_prob(df):
    conditional_prob = df[df['ID_INPUT'] == 4]['VALUE'].value_counts() / len(df[df['ID_INPUT'] == 4])
    return conditional_prob
```

- this is not the formula of conditional probability (i.e. there's no "given" term, which is represented by this symbol |)
- this is just a regular probability of appearance of an executable file over all possible executables
- no usage in the transition matrix, so excluded it

2/. for split_train_test: test size = 20% (you put 30%)

3/. 

```
def get_transitional_matrix(trans_prob, X):
    all_exes = get_unique_states(X)
    probs_for_matrix = []
    for col in all_exes:
        exe_probs = []
        for row in all_exes:
            pair = (col, row)
            if pair in trans_prob:
                exe_probs.append(trans_prob[pair])
            else:
                exe_probs.append(0)
        probs_for_matrix.append(exe_probs)
    
    matrix = pd.DataFrame(probs_for_matrix, index = all_exes, columns = all_exes)
    return matrix
```
- note that: the transition probability is going from row to col ==> row happens first, then col happens later ==> pair should be pair = (row,col), but you put pair=(col,row)

4/. Just terminology: transitional prob/matrix maybe the correct grammar, but let's just put transition probability/matrix due to the mathematical aspect.

5/. typo in the return of predict_HMM
- transition_matrix, not matrix
