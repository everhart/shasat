# shasat
Reductions of SHA family hash functions to SAT.

Given a hash function, a digest and the size of some unknown message, a Boolean expression is generated representing the computational circuit used by the specified hash function to hash the unknown message and produce the described digest. Determining a satisfiable interpretation to the resultant Boolean expression is thereby equivelant to reverse engineering the specified hash function.

## Usage 
`./shasat [options]`

## Options
`
  --help		Display this information
  --sha1		Use SHA1 as the hash function
  --sha256		Use SHA2-256 as the hash function
  --sha224		Use SHA2-224 as the has function
  -o <file>		Save the output Boolean expression in dimacs cnf form to <file>
  -d <digest>		Use <digest> as the output digest
  -m <message_size>	Use <message_size> as the size of the unknown input message
`
## Example: 
`./shasat -o dimacs.cnf --sha256 -d e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 -m 14`
