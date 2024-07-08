import click
import cmds


@click.group()
def cli() -> None:
    pass


@cli.command()
def update_av_data() -> None:
    cmds.update_av_data.main_v1()


if __name__ == "__main__":
    cli()
