import logging

import alphvant
import alphvant.browse_symbols
import click
import cmds
import stocktotum
import stocktotum.sma


@click.group()
def cli() -> None:
    pass


@cli.command()
def update_av_data() -> None:
    cmds.update_av_data.main_v1()


@cli.command()
@click.argument("symbol")
def graph_sma(symbol: str) -> None:
    cmds.graph_sma.main(symbol)


@cli.command()
@click.argument("symbol")
@click.argument("startdate", required=False)
def graph_mr(symbol: str, startdate: str | None) -> None:
    if startdate is None:
        startdate = cmds.graph_mr.get_n_years_ago_date(2)
    cmds.graph_mr.main(symbol, startdate)


@cli.command()
@click.argument("startdate", required=False)
def back_test_mr(startdate: str | None) -> None:
    if startdate is None:
        startdate = cmds.graph_mr.get_n_years_ago_date(2)
    cmds.back_test.mr(20, startdate=startdate)


@cli.command()
@click.argument("startdate", required=False)
def back_test_pt_mr(startdate: str | None) -> None:
    if startdate is None:
        startdate = cmds.graph_mr.get_n_years_ago_date(2)
    cmds.back_test.pt_mr(50, startdate=startdate)


@cli.command()
@click.argument("startdate", required=False)
def graph_pt(startdate: str | None) -> None:
    if startdate is None:
        startdate = cmds.graph_mr.get_n_years_ago_date(2)
    cmds.graph_pt.main()


@cli.command()
@click.argument("startdate")
@click.argument("symbols", nargs=-1)
def graph_many(startdate: str, symbols: list[str]) -> None:
    cmds.graph_many.main(startdate, symbols)


@cli.command()
def compute_volumous_symbols() -> None:
    alphvant.browse_symbols.compute_voluous_symbols()


def init_logging() -> None:
    # Set up basic configuration for logging
    logging.basicConfig(
        level=logging.INFO,  # Set the log level
        format="[%(asctime)s]%(name)s - %(levelname)s - %(message)s",  # Set the log format
        datefmt="%Y-%m-%d %H:%M:%S",  # Set the date format
    )


if __name__ == "__main__":
    init_logging()
    cli()
