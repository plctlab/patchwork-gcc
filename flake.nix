{
  description = "gcc compiler";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = {
          default = pkgs.gnumake;
        };
        formatter = pkgs.nixpkgs-fmt;

        devShell = pkgs.mkShell {
          buildInputs = [
            pkgs.gnumake
            pkgs.gcc13

            pkgs.gmp
            pkgs.libmpc
            pkgs.mpfr
            pkgs.isl
            pkgs.pkg-config
            pkgs.autoconf-archive
            pkgs.autoconf
            pkgs.automake
          ];
        };
      }
    );
}
