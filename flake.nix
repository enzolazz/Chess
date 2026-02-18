{
  description = "Chess game built with SFML";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = { self, nixpkgs, flake-utils, nixgl }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        nixGL = nixgl.packages.${system}.nixGLIntel;
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "chess-app";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            makeWrapper
          ];

          buildInputs = with pkgs; [
            sfml
          ];

          configurePhase = ''
            cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
          '';

          buildPhase = ''
            cmake --build build
          '';

          installPhase = ''
            mkdir -p $out/bin $out/share/chess-app
            cp build/ChessApp $out/bin/.ChessApp-unwrapped
            cp -r etc $out/share/chess-app/

            makeWrapper ${nixGL}/bin/nixGLIntel $out/bin/ChessApp \
              --add-flags "$out/bin/.ChessApp-unwrapped" \
              --chdir $out/share/chess-app
          '';

          meta.mainProgram = "ChessApp";
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            sfml
            gcc
          ];
        };
      }
    );
}
